package LycorisNet

import (
	"runtime"
	"sync"
	"container/list"
	"sort"
)

// The structure lycoris is the highest level module of LycorisNet,
// and it's in singleton pattern.
type lycoris struct {
	speciesList []species            // The slice of species.
	forwardFunc func(in *Individual) // This core function needs to be completed.
	Best        Individual           // The best individual.
	Capacity    int                  // The capacity of lycoris.
	InputNum    int                  // The dimension of input.
	OutputNum   int                  // The dimension of output.
}

var tempList1 [][]Individual // A temporary two-dimensional slice to store individuals.
var tempList2 [][]int        // A temporary slice used in classify().
var tick int                 // Used in autoParameter().
var tock int                 // Used in autoParameter().
var hit int                  // Used in autoParameter().
var miss int                 // Used in autoParameter().
var gapList *list.List       // The role is to automatically change some of the parameters.

// Apply the core function to lycoris. It is driven by events and
// promotes the fitting of the whole network.
func (radiata *lycoris) SetForwardFunc(f func(in *Individual)) {
	radiata.forwardFunc = f
}

// It's for singleton pattern.
var radiata *lycoris
var once sync.Once

// Emerge a new object.
func NewLycoris(capacity int, inputNum int, outputNum int) *lycoris {
	once.Do(func() {
		runtime.GOMAXPROCS(cpuNum)
		go randFloat32() // Init the random number generator.

		// Emerge a new lycoris and set some parameters.
		radiata = &lycoris{}
		radiata.Capacity = capacity
		radiata.InputNum = inputNum
		radiata.OutputNum = outputNum
		tock = 1 // Minimum step.
		gapList = list.New()
		var specie = species{}
		var initialCapacity = int(float32(capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
		specie.individualList = make([]Individual, initialCapacity)
		for i := 0; i < initialCapacity; i++ {
			specie.individualList[i] = *newIndividual(inputNum, outputNum)
		}
		radiata.speciesList = append(radiata.speciesList, specie)
	})

	return radiata
}

// Assist in parallel computing.
var wait sync.WaitGroup

// The length of speciesList.
var specieLength int
// Store the length of individualList in speciesList.
var oldLength []int

// Mating.
func (radiata *lycoris) mate() {
	specieLength = len(radiata.speciesList)
	tempList1 = make([][]Individual, specieLength)
	oldLength = make([]int, specieLength)
	var start = make([][]int, cpuNum)
	var end = make([][]int, cpuNum)
	for i := 0; i < cpuNum; i++ {
		start[i] = make([]int, specieLength)
		end[i] = make([]int, specieLength)
	}

	for i := 0; i < specieLength; i++ {
		var individualLength = len(radiata.speciesList[i].individualList)
		oldLength[i] = individualLength
		var mateLength = int(float32(individualLength) * mateOdds)
		tempList1[i] = make([]Individual, mateLength)
		var part = mateLength / cpuNum
		var temp = cpuNum - 1
		for j := 0; j < temp; j++ {
			start[j][i] = j * part
			end[j][i] = (j + 1) * part
		}
		start[temp][i] = temp * part
		end[temp][i] = mateLength
	}

	wait.Add(cpuNum)
	for i := 0; i < cpuNum; i++ {
		go radiata.mateCore(start[i], end[i])
	}
	wait.Wait()
}

// The parallel kernel of mate().
func (radiata *lycoris) mateCore(start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		var l = radiata.speciesList[i].individualList
		for j := start[i]; j < end[i]; j++ {
			tempList1[i][j] = *mateIndividual(&(l[LycorisRandomInt(oldLength[i])]), &(l[LycorisRandomInt(oldLength[i])]))
		}
	}
	wait.Done()
}

// Mutating.
func (radiata *lycoris) mutate() {
	specieLength = len(radiata.speciesList)
	tempList1 = make([][]Individual, specieLength)
	oldLength = make([]int, specieLength)
	var start = make([][]int, cpuNum)
	var end = make([][]int, cpuNum)
	for i := 0; i < cpuNum; i++ {
		start[i] = make([]int, specieLength)
		end[i] = make([]int, specieLength)
	}

	for i := 0; i < specieLength; i++ {
		var individualLength = len(radiata.speciesList[i].individualList)
		oldLength[i] = individualLength
		var mutateLength = int(float32(individualLength) * mutateOdds)
		tempList1[i] = make([]Individual, mutateLength)
		var part = mutateLength / cpuNum
		var temp = cpuNum - 1
		for j := 0; j < temp; j++ {
			start[j][i] = j * part
			end[j][i] = (j + 1) * part
		}
		start[temp][i] = temp * part
		end[temp][i] = mutateLength
	}

	wait.Add(cpuNum)
	for i := 0; i < cpuNum; i++ {
		go radiata.mutateCore(start[i], end[i])
	}
	wait.Wait()
}

// The parallel kernel of mutate().
func (radiata *lycoris) mutateCore(start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		var l = radiata.speciesList[i].individualList
		for j := start[i]; j < end[i]; j++ {
			tempList1[i][j] = *mutateIndividual(&(l[LycorisRandomInt(oldLength[i])]))
		}
	}
	wait.Done()
}

// Computing distances between individuals. And then classify them.
func (radiata *lycoris) classify() {
	tempList2 = make([][]int, specieLength)
	var start = make([][]int, cpuNum)
	var end = make([][]int, cpuNum)
	for i := 0; i < cpuNum; i++ {
		start[i] = make([]int, specieLength)
		end[i] = make([]int, specieLength)
	}

	for i := 0; i < specieLength; i++ {
		var tempList1Length = len(tempList1[i])
		tempList2[i] = make([]int, tempList1Length)
		var part = tempList1Length / cpuNum
		var temp = cpuNum - 1
		for j := 0; j < temp; j++ {
			start[j][i] = j * part
			end[j][i] = (j + 1) * part
		}
		start[temp][i] = temp * part
		end[temp][i] = tempList1Length
	}

	wait.Add(cpuNum)
	for i := 0; i < cpuNum; i++ {
		go radiata.classifyCore(start[i], end[i])
	}
	wait.Wait()

	radiata.speciesList = append(radiata.speciesList, species{})
	for k1, v1 := range tempList2 {
		for k2, v2 := range v1 {
			if v2 != -1 {
				radiata.speciesList[v2].individualList = append(radiata.speciesList[v2].individualList, tempList1[k1][k2])
			} else {
				radiata.speciesList[specieLength].individualList = append(radiata.speciesList[specieLength].individualList, tempList1[k1][k2])
			}
		}
	}

	var tempSpeciesList []species
	for _, v := range radiata.speciesList {
		if len(v.individualList) != 0 {
			tempSpeciesList = append(tempSpeciesList, v)
		}
	}
	radiata.speciesList = tempSpeciesList
}

// The parallel kernel of classify().
func (radiata *lycoris) classifyCore(start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		for j := start[i]; j < end[i]; j++ {
			var temp = &(tempList1[i][j])
			for k := 0; k < specieLength; k++ {
				var l = radiata.speciesList[k].individualList
				var dis = distance(temp, &(l[LycorisRandomInt(oldLength[k])]))
				if dis <= distanceThreshold {
					tempList2[i][j] = k
					break
				} else {
					tempList2[i][j] = -1
				}
			}
		}
	}
	wait.Done()
}

// All individuals are calculated forward.
func (radiata *lycoris) forward() {
	specieLength = len(radiata.speciesList)
	var start = make([][]int, cpuNum)
	var end = make([][]int, cpuNum)
	for i := 0; i < cpuNum; i++ {
		start[i] = make([]int, specieLength)
		end[i] = make([]int, specieLength)
	}

	for i := 0; i < specieLength; i++ {
		var individualLength = len(radiata.speciesList[i].individualList)
		var part = individualLength / cpuNum
		var temp = cpuNum - 1
		for j := 0; j < temp; j++ {
			start[j][i] = j * part
			end[j][i] = (j + 1) * part
		}
		start[temp][i] = temp * part
		end[temp][i] = individualLength
	}

	wait.Add(cpuNum)
	for i := 0; i < cpuNum; i++ {
		go radiata.forwardCore(start[i], end[i])
	}
	wait.Wait()
}

// The parallel kernel of forward().
func (radiata *lycoris) forwardCore(start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		for j := start[i]; j < end[i]; j++ {
			radiata.forwardFunc(&(radiata.speciesList[i].individualList[j]))
		}
	}
	wait.Done()
}

var p1B float32         // The backup of p1.
var p2B float32         // The backup of p2.
var p3B float32         // The backup of p3.
var p4B float32         // The backup of p4.
var p5B float32         // The backup of p5.
var p6B float32         // The backup of p6.
var mateOddsB float32   // The backup of mateOdds.
var mutateOddsB float32 // The backup of mutateOdds.
var mutateTimeB int     // The backup of mutateTime.

func emergeArgs() { // Memory exceeds the limit.
	if memOverFlag {
		var mutateTimeE = LycorisRandomInt(10) + 1 // [1, 10]
		var mateOddsE = LycorisRandomFloat32()     // [0.0, 1.0)
		var mutateOddsE = LycorisRandomFloat32()   // [0.0, 1.0)
		var remain float32 = 1
		var p1E float32 = 0
		remain -= p1E
		var p2E = LycorisRandomFloat32() * remain
		remain -= p2E
		var p3E float32 = 0
		remain -= p3E
		var p4E = LycorisRandomFloat32() * remain
		remain -= p4E
		var p5E float32 = 0
		remain -= p5E
		var p6E = remain

		if firstOver {
			firstOver = false
			p1B, p2B, p3B, p4B, p5B, p6B, mateOddsB, mutateOddsB, mutateTimeB = 0, p2E, 0, p4E, 0, p6E, mateOdds, mutateOdds, mutateTime
			p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1E, p2E, p3E, p4E, p5E, p6E, mateOddsE, mutateOddsE, mutateTimeE
		} else {
			p1B, p2B, p3B, p4B, p5B, p6B, mateOddsB, mutateOddsB, mutateTimeB = p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime
			p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1E, p2E, p3E, p4E, p5E, p6E, mateOddsE, mutateOddsE, mutateTimeE
		}

	} else {
		var mutateTimeE = LycorisRandomInt(10) + 1 // [1, 10]
		var mateOddsE = LycorisRandomFloat32()     // [0.0, 1.0)
		var mutateOddsE = LycorisRandomFloat32()   // [0.0, 1.0)
		var remain float32 = 1
		var p1E = LycorisRandomFloat32()
		remain -= p1E
		var p2E = LycorisRandomFloat32() * remain
		remain -= p2E
		var p3E = LycorisRandomFloat32() * remain
		remain -= p3E
		var p4E = LycorisRandomFloat32() * remain
		remain -= p4E
		var p5E = LycorisRandomFloat32() * remain
		remain -= p5E
		var p6E = remain

		p1B, p2B, p3B, p4B, p5B, p6B, mateOddsB, mutateOddsB, mutateTimeB = p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime
		p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1E, p2E, p3E, p4E, p5E, p6E, mateOddsE, mutateOddsE, mutateTimeE
	}
}

// Assist in autoParameter().
var checkFlag = false

// Update some parameters automatically.
func autoParameter() {
	if checkFlag {
		var length = gapList.Len()
		var lastValue = gapList.Back().Value.(float32)
		var count = 0
		for e := gapList.Front(); e != nil; e = e.Next() {
			if lastValue > e.Value.(float32) {
				count++
			}
		}

		if count < (length/2 + 1) { // Miss.
			miss++
			hit = 0
			if miss == 2 {
				if tock < 16 { // Maximum step.
					tock *= 2
				}
				miss = 1
			}
			p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1B, p2B, p3B, p4B, p5B, p6B, mateOddsB, mutateOddsB, mutateTimeB
		} else { // Hit.
			hit++
			miss = 0
			if hit == 2 {
				if tock > 1 { // Minimum step.
					tock /= 2
				}
				hit = 1
			}
		}
		checkFlag = false
	}

	if tick == tock {
		emergeArgs()
		checkFlag = true
		tick = 0
	} else {
		tick += 1
	}
}

// Assist in chooseElite().
var gapListFlag = true

// Choose elites and manipulate gapList.
func (radiata *lycoris) chooseElite() {
	var totalLength = 0
	for _, v := range radiata.speciesList {
		totalLength += len(v.individualList)
	}
	var sortList = (sortFitnessArray)(make([]sortFitness, totalLength))
	var pointer = 0
	for k1, v1 := range radiata.speciesList {
		for k2, v2 := range v1.individualList {
			sortList[pointer] = sortFitness{v2.Fitness, k1, k2}
			pointer++
		}
	}
	sort.Sort(sortList)
	var last = sortList[totalLength-1]
	var tempBest = radiata.speciesList[last.specieNum].individualList[last.individualNum]

	var newSpecieList = make([]species, specieLength)
	var newLength = int(float32(radiata.Capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	var memSum = 0
	var i = totalLength - 1
	for ; i > totalLength-newLength-1; i-- {
		if i == -1 { // TODO: Elegant repair.
			break
		}
		var temp = sortList[i]
		var tempIndividual = radiata.speciesList[temp.specieNum].individualList[temp.individualNum]
		if memLimitFlag {
			memSum += tempIndividual.getSize()
		}
		newSpecieList[temp.specieNum].individualList = append(newSpecieList[temp.specieNum].individualList, tempIndividual)
	}
	if memLimitFlag {
		if memSum > (totalLength-i-1)*limitSize { // Memory exceeds the limit.
			if !memOverFlag {
				firstOver = true
				memOverFlag = true
			}
		} else {
			memOverFlag = false
		}
	}

	var tempSpeciesList []species
	for _, v := range newSpecieList {
		if len(v.individualList) != 0 {
			tempSpeciesList = append(tempSpeciesList, v)
		}
	}
	radiata.speciesList = tempSpeciesList

	var difference = tempBest.Fitness - radiata.Best.Fitness
	radiata.Best = tempBest
	// len < 8
	if gapListFlag {
		var length = gapList.Len()
		if length == 7 {
			gapListFlag = false
		}
		gapList.PushBack(difference)
	} else { // len == 8
		gapList.Remove(gapList.Front())
		gapList.PushBack(difference)
	}
}

// Each time this function is called, the network runs forward one time.
func (radiata *lycoris) RunLycoris() {
	radiata.mate()        // Mating.
	radiata.classify()    // Computing distances of new individuals.
	radiata.mutate()      // Mutating.
	radiata.classify()    // Computing distances of new individuals.
	radiata.forward()     // Forward calculation.
	autoParameter()       // Changing some parameters automatically.
	radiata.chooseElite() // Sorting and choosing some individuals with higher fitness.
}

// Import the individual and emerge a new object.
func ImportLycoris(path string, capacity int) *lycoris {
	runtime.GOMAXPROCS(cpuNum)
	go randFloat32() // Init the random number generator.

	// Import the individual.
	var source = ImportIndividual(path)
	// Emerge a new lycoris and set some parameters.
	radiata = &lycoris{}
	radiata.Capacity = capacity
	radiata.InputNum = source.InputNum
	radiata.OutputNum = source.OutputNum
	tock = 1 // Minimum step.
	gapList = list.New()
	var specie = species{}
	var initialCapacity = int(float32(capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	specie.individualList = make([]Individual, initialCapacity)
	for i := 0; i < initialCapacity; i++ {
		specie.individualList[i] = *(source.clone())
	}
	radiata.speciesList = append(radiata.speciesList, specie)
	return radiata
}

var memLimitFlag = false // The flag of memory limit.
var limitSize int        // The size of maximum memory.
var memOverFlag = false  // When memory exceeds the limit, this changes to true.
var firstOver = false    // To fixed a potential bug.

// Turn on memory-limit.
func (radiata *lycoris) OpenMemLimit(size int) {
	limitSize = size * 9 / 10
	memLimitFlag = true
}

// Turn off memory-limit.
func (radiata *lycoris) CloseMemLimit() {
	memLimitFlag = false
	memOverFlag = false
	firstOver = false
}
