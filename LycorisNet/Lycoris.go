package LycorisNet

import (
	"runtime"
	"sync"
	"container/list"
	"sort"
)

type lycoris struct {
	speciesList    []species            // The slice of species.
	forwardFunc    func(in *Individual) // This function needs to be completed.
	Best           Individual           // The best individual.
	bufferList     [][]Individual
	tempDistance   [][]int
	tick           int
	tock           int
	hit            int
	miss           int
	differenceList *list.List
	capacity       int
	inputNum       int
	outputNum      int
}

func (radiata *lycoris) SetForwardFunc(f func(in *Individual)) {
	radiata.forwardFunc = f
}

func NewLycoris(capacity int, inputNum int, outputNum int) *lycoris {
	runtime.GOMAXPROCS(runtime.NumCPU())
	go randFloat32() // Init the random number generator.
	var radiata = &lycoris{}
	radiata.capacity = capacity
	radiata.inputNum = inputNum
	radiata.outputNum = outputNum
	radiata.tock = 1
	radiata.differenceList = list.New()
	var specie = species{}
	var initialCapacity = int(float32(capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	specie.individualList = make([]Individual, initialCapacity)
	for i := 0; i < initialCapacity; i++ {
		specie.individualList[i] = *newIndividual(inputNum, outputNum)
	}
	radiata.speciesList = append(radiata.speciesList, specie)
	return radiata
}

var wait sync.WaitGroup

func (radiata *lycoris) mate() {
	var specieLength = len(radiata.speciesList)
	radiata.bufferList = make([][]Individual, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(radiata.speciesList[i].individualList)
		var mateLength = int(float32(individualLength) * mateOdds)
		radiata.bufferList[i] = make([]Individual, mateLength)
		var part = mateLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go radiata.mate_core(i, j*part, (j+1)*part)
		}
		go radiata.mate_core(i, (cpuNum-1)*part, mateLength)
		wait.Wait()
	}
}

func (radiata *lycoris) mate_core(specieNum int, start int, end int) {
	var l = radiata.speciesList[specieNum].individualList
	var length = len(l)
	for i := start; i < end; i++ {
		radiata.bufferList[specieNum][i] = *mateIndividual(&(l[GetRandomInt(length)]), &(l[GetRandomInt(length)]))
	}
	wait.Done()
}

func (radiata *lycoris) mutate() {
	var specieLength = len(radiata.speciesList)
	radiata.bufferList = make([][]Individual, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(radiata.speciesList[i].individualList)
		var mutateLength = int(float32(individualLength) * mutateOdds)
		radiata.bufferList[i] = make([]Individual, mutateLength)
		var part = mutateLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go radiata.mutate_core(i, j*part, (j+1)*part)
		}
		go radiata.mutate_core(i, (cpuNum-1)*part, mutateLength)
		wait.Wait()
	}
}

func (radiata *lycoris) mutate_core(specieNum int, start int, end int) {
	var l = radiata.speciesList[specieNum].individualList
	var length = len(l)
	for i := start; i < end; i++ {
		radiata.bufferList[specieNum][i] = *mutateIndividual(&(l[GetRandomInt(length)]))
	}
	wait.Done()
}

func (radiata *lycoris) classify() {
	var specieLength = len(radiata.speciesList)
	radiata.tempDistance = make([][]int, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var bufferLength = len(radiata.bufferList[i])
		radiata.tempDistance[i] = make([]int, bufferLength)
		var part = bufferLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go radiata.classify_core(i, j*part, (j+1)*part)
		}
		go radiata.classify_core(i, (cpuNum-1)*part, bufferLength)
		wait.Wait()
	}
	radiata.speciesList = append(radiata.speciesList, species{})
	for k1, v1 := range radiata.tempDistance {
		for k2, v2 := range v1 {
			if v2 != -1 {
				radiata.speciesList[v2].individualList = append(radiata.speciesList[v2].individualList, radiata.bufferList[k1][k2])
			} else {
				radiata.speciesList[specieLength].individualList = append(radiata.speciesList[specieLength].individualList, radiata.bufferList[k1][k2])
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

func (radiata *lycoris) classify_core(specieNum int, start int, end int) {
	var specieLength = len(radiata.speciesList)
	for i := start; i < end; i++ {
		for j := 0; j < specieLength; j++ {
			var list = radiata.speciesList[j].individualList
			var dis = distance(&(radiata.bufferList[specieNum][i]), &(list[GetRandomInt(len(list))]))
			if dis <= distanceThreshold {
				radiata.tempDistance[specieNum][i] = j
				break
			} else {
				radiata.tempDistance[specieNum][i] = -1
			}
		}
	}
	wait.Done()
}

func (radiata *lycoris) forward() {
	var specieLength = len(radiata.speciesList)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(radiata.speciesList[i].individualList)
		var part = individualLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go radiata.forward_core(i, j*part, (j+1)*part)
		}
		go radiata.forward_core(i, (cpuNum-1)*part, individualLength)
		wait.Wait()
	}
}

func (radiata *lycoris) forward_core(specieNum int, start int, end int) {
	for i := start; i < end; i++ {
		radiata.forwardFunc(&(radiata.speciesList[specieNum].individualList[i]))
	}
	wait.Done()
}

var p1_b float32
var p2_b float32
var p3_b float32
var p4_b float32
var p5_b float32
var p6_b float32
var mateOdds_b float32
var mutateOdds_b float32
var mutateTime_b int

func emergeArgs() {
	var mutateTime_e = GetRandomInt(10) + 1
	var mateOdds_e = GetRandomFloat32()
	var mutateOdds_e = GetRandomFloat32()
	var remain float32 = 1
	var p1_e = GetRandomFloat32()
	remain -= p1_e
	var p2_e = GetRandomFloat32() * remain
	remain -= p2_e
	var p3_e = GetRandomFloat32() * remain
	remain -= p3_e
	var p4_e = GetRandomFloat32() * remain
	remain -= p4_e
	var p5_e = GetRandomFloat32() * remain
	remain -= p5_e
	var p6_e = remain
	p1_b, p2_b, p3_b, p4_b, p5_b, p6_b, mateOdds_b, mutateOdds_b, mutateTime_b = p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime
	p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1_e, p2_e, p3_e, p4_e, p5_e, p6_e, mateOdds_e, mutateOdds_e, mutateTime_e
}

var checkFlag = false

func (radiata *lycoris) autoParameter() {
	if checkFlag {
		var length = radiata.differenceList.Len()
		var lastValue = radiata.differenceList.Back().Value.(float32)
		var count = 0
		for e := radiata.differenceList.Front(); e != nil; e = e.Next() {
			if lastValue >= e.Value.(float32) {
				count++
			}
		}
		if count < (length/2 + 1) { // Miss.
			radiata.miss++
			radiata.hit = 0
			if radiata.miss == 2 {
				if radiata.tock < 512 {
					radiata.tock *= 2
				}
				radiata.miss = 1
			}
			p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1_b, p2_b, p3_b, p4_b, p5_b, p6_b, mateOdds_b, mutateOdds_b, mutateTime_b
		} else { // Hit.
			radiata.hit++
			radiata.miss = 0
			if radiata.hit == 2 {
				if radiata.tock > 1 {
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

type sort_fitness struct {
	fitness       float32
	specieNum     int
	individualNum int
}

type sort_fitness_array []sort_fitness

func (f sort_fitness_array) Len() int {
	return len(f)
}

func (f sort_fitness_array) Less(i, j int) bool {
	return f[i].fitness < f[j].fitness
}

func (f sort_fitness_array) Swap(i, j int) {
	f[i], f[j] = f[j], f[i]
}

var differenceListFlag = true

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

	var specieLength = len(radiata.speciesList)
	var newSpecieList = make([]species, specieLength)
	var newLength = int(float32(radiata.capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	for i := totalLength - 1; i > totalLength-newLength-1; i-- {
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
	if differenceListFlag {
		var length = radiata.differenceList.Len()
		if length == 7 {
			differenceListFlag = false
		}
		radiata.differenceList.PushBack(difference)
	} else { // len == 8
		radiata.differenceList.Remove(radiata.differenceList.Front())
		radiata.differenceList.PushBack(difference)
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
