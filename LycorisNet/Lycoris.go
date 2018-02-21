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
	tempList1   [][]Individual       // A temporary two-dimensional slice to store individuals.
	tempList2   [][]int              // A temporary slice used in classify().
	tick        int                  // Used in autoParameter().
	tock        int                  // Used in autoParameter().
	hit         int                  // Used in autoParameter().
	miss        int                  // Used in autoParameter().
	gapList     *list.List           // The role is to automatically change some of the parameters.
	Capacity    int                  // The capacity of lycoris.
	InputNum    int                  // The dimension of input.
	OutputNum   int                  // The dimension of output.
}

// Apply the core function to lycoris. It is driven by events and
// promotes the fitting of the whole network.
func (radiata *lycoris) SetForwardFunc(f func(in *Individual)) {
	radiata.forwardFunc = f
}

// It's for singleton pattern.
var radiata *lycoris

// Emerge new object.
func NewLycoris(capacity int, inputNum int, outputNum int) *lycoris {
	if radiata == nil {
		runtime.GOMAXPROCS(cpuNum)
		go randFloat32() // Init the random number generator.

		// Emerge a new lycoris and set some parameters.
		radiata = &lycoris{}
		radiata.Capacity = capacity
		radiata.InputNum = inputNum
		radiata.OutputNum = outputNum
		radiata.tock = 1 // Minimum step.
		radiata.gapList = list.New()
		var specie = species{}
		var initialCapacity = int(float32(capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
		specie.individualList = make([]Individual, initialCapacity)
		for i := 0; i < initialCapacity; i++ {
			specie.individualList[i] = *newIndividual(inputNum, outputNum)
		}
		radiata.speciesList = append(radiata.speciesList, specie)
	}
	return radiata
}

// Assist in parallel computing.
var wait sync.WaitGroup

// The length of speciesList.
var specieLength int

// Mating.
func (radiata *lycoris) mate() {
	specieLength = len(radiata.speciesList)
	radiata.tempList1 = make([][]Individual, specieLength)
	var oldLength = make([]int, specieLength)
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
		radiata.tempList1[i] = make([]Individual, mateLength)
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
		go radiata.mate_core(oldLength, start[i], end[i])
	}
	wait.Wait()
}

// The parallel kernel of mate().
func (radiata *lycoris) mate_core(oldLength []int, start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		var l = radiata.speciesList[i].individualList
		for j := start[i]; j < end[i]; j++ {
			radiata.tempList1[i][j] = *mateIndividual(&(l[LycorisRandomInt(oldLength[i])]), &(l[LycorisRandomInt(oldLength[i])]))
		}
	}
	wait.Done()
}

// Mutating.
func (radiata *lycoris) mutate() {
	specieLength = len(radiata.speciesList)
	radiata.tempList1 = make([][]Individual, specieLength)
	var oldLength = make([]int, specieLength)
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
		radiata.tempList1[i] = make([]Individual, mutateLength)
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
		go radiata.mutate_core(oldLength, start[i], end[i])
	}
	wait.Wait()
}

// The parallel kernel of mutate().
func (radiata *lycoris) mutate_core(oldLength []int, start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		var l = radiata.speciesList[i].individualList
		for j := start[i]; j < end[i]; j++ {
			radiata.tempList1[i][j] = *mutateIndividual(&(l[LycorisRandomInt(oldLength[i])]))
		}
	}
	wait.Done()
}

// Computing distances between individuals. And then classify them.
func (radiata *lycoris) classify() {
	specieLength = len(radiata.speciesList)
	radiata.tempList2 = make([][]int, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var bufferLength = len(radiata.tempList1[i])
		radiata.tempList2[i] = make([]int, bufferLength)
		var part = bufferLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go radiata.classify_core(i, j*part, (j+1)*part)
		}
		go radiata.classify_core(i, (cpuNum-1)*part, bufferLength)
		wait.Wait()
	}
	radiata.speciesList = append(radiata.speciesList, species{})
	for k1, v1 := range radiata.tempList2 {
		for k2, v2 := range v1 {
			if v2 != -1 {
				radiata.speciesList[v2].individualList = append(radiata.speciesList[v2].individualList, radiata.tempList1[k1][k2])
			} else {
				radiata.speciesList[specieLength].individualList = append(radiata.speciesList[specieLength].individualList, radiata.tempList1[k1][k2])
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
func (radiata *lycoris) classify_core(specieNum int, start int, end int) {
	specieLength = len(radiata.speciesList)
	for i := start; i < end; i++ {
		for j := 0; j < specieLength; j++ {
			var list = radiata.speciesList[j].individualList
			var dis = distance(&(radiata.tempList1[specieNum][i]), &(list[LycorisRandomInt(len(list))]))
			if dis <= distanceThreshold {
				radiata.tempList2[specieNum][i] = j
				break
			} else {
				radiata.tempList2[specieNum][i] = -1
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
		go radiata.forward_core(start[i], end[i])
	}
	wait.Wait()
}

// The parallel kernel of forward().
func (radiata *lycoris) forward_core(start []int, end []int) {
	for i := 0; i < specieLength; i++ {
		for j := start[i]; j < end[i]; j++ {
			radiata.forwardFunc(&(radiata.speciesList[i].individualList[j]))
		}
	}
	wait.Done()
}

var p1_b float32         // The backup of p1.
var p2_b float32         // The backup of p2.
var p3_b float32         // The backup of p3.
var p4_b float32         // The backup of p4.
var p5_b float32         // The backup of p5.
var p6_b float32         // The backup of p6.
var mateOdds_b float32   // The backup of mateOdds.
var mutateOdds_b float32 // The backup of mutateOdds.
var mutateTime_b int     // The backup of mutateTime.

func emergeArgs() {
	var mutateTime_e = LycorisRandomInt(10) + 1 // [1, 10]
	var mateOdds_e = LycorisRandomFloat32()     // [0.0, 1.0)
	var mutateOdds_e = LycorisRandomFloat32()   // [0.0, 1.0)
	var remain float32 = 1
	var p1_e = LycorisRandomFloat32()
	remain -= p1_e
	var p2_e = LycorisRandomFloat32() * remain
	remain -= p2_e
	var p3_e = LycorisRandomFloat32() * remain
	remain -= p3_e
	var p4_e = LycorisRandomFloat32() * remain
	remain -= p4_e
	var p5_e = LycorisRandomFloat32() * remain
	remain -= p5_e
	var p6_e = remain

	p1_b, p2_b, p3_b, p4_b, p5_b, p6_b, mateOdds_b, mutateOdds_b, mutateTime_b = p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime
	p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1_e, p2_e, p3_e, p4_e, p5_e, p6_e, mateOdds_e, mutateOdds_e, mutateTime_e
}

// Assist in autoParameter().
var checkFlag = false

// Update some parameters automatically.
func (radiata *lycoris) autoParameter() {
	if checkFlag {
		var length = radiata.gapList.Len()
		var lastValue = radiata.gapList.Back().Value.(float32)
		var count = 0
		for e := radiata.gapList.Front(); e != nil; e = e.Next() {
			if lastValue >= e.Value.(float32) {
				count++
			}
		}

		if count < (length/2 + 1) { // Miss.
			radiata.miss++
			radiata.hit = 0
			if radiata.miss == 2 {
				if radiata.tock < 16 { // Maximum step.
					radiata.tock *= 2
				}
				radiata.miss = 1
			}
			p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1_b, p2_b, p3_b, p4_b, p5_b, p6_b, mateOdds_b, mutateOdds_b, mutateTime_b
		} else { // Hit.
			radiata.hit++
			radiata.miss = 0
			if radiata.hit == 2 {
				if radiata.tock > 1 { // Minimum step.
					radiata.tock /= 2
				}
				radiata.hit = 1
			}
		}
		checkFlag = false
	}

	if radiata.tick == radiata.tock {
		emergeArgs()
		checkFlag = true
		radiata.tick = 0
	} else {
		radiata.tick += 1
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
	var sortList = (sort_fitness_array)(make([]sort_fitness, totalLength))
	var pointer = 0
	for k1, v1 := range radiata.speciesList {
		for k2, v2 := range v1.individualList {
			sortList[pointer] = sort_fitness{v2.Fitness, k1, k2}
			pointer++
		}
	}
	sort.Sort(sortList)
	var last = sortList[totalLength-1]
	var tempBest = radiata.speciesList[last.specieNum].individualList[last.individualNum]

	specieLength = len(radiata.speciesList)
	var newSpecieList = make([]species, specieLength)
	var newLength = int(float32(radiata.Capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	for i := totalLength - 1; i > totalLength-newLength-1; i-- {
		if i == -1 {
			break
		}
		var temp = sortList[i]
		newSpecieList[temp.specieNum].individualList = append(newSpecieList[temp.specieNum].individualList, radiata.speciesList[temp.specieNum].individualList[temp.individualNum])
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
		var length = radiata.gapList.Len()
		if length == 7 {
			gapListFlag = false
		}
		radiata.gapList.PushBack(difference)
	} else { // len == 8
		radiata.gapList.Remove(radiata.gapList.Front())
		radiata.gapList.PushBack(difference)
	}
}

// Each time this function is called, the network runs forward one time.
func (radiata *lycoris) RunLycoris() {
	radiata.mate()          // Mating.
	radiata.classify()      // Computing distances of new individuals.
	radiata.mutate()        // Mutating.
	radiata.classify()      // Computing distances of new individuals.
	radiata.forward()       // Forward calculation.
	radiata.autoParameter() // Changing some parameters automatically.
	radiata.chooseElite()   // Sorting and choosing some individuals with higher fitness.
}
