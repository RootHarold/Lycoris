package LycorisNet

import (
	"runtime"
	"sync"
	"container/list"
	"sort"
)

type Lycoris struct {
	speciesList    []species            // The slice of species.
	forwardFunc    func(in *Individual) // This function needs to be completed.
	best           Individual           // The best individual.
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

func (lycoris *Lycoris) SetForwardFunc(f func(in *Individual)) {
	lycoris.forwardFunc = f
}

func NewLycoris(capacity int, inputNum int, outputNum int) *Lycoris {
	runtime.GOMAXPROCS(runtime.NumCPU())
	go randFloat32() // Init the random number generator.
	var lycoris = &Lycoris{}
	lycoris.capacity = capacity
	lycoris.inputNum = inputNum
	lycoris.outputNum = outputNum
	lycoris.tock = 1
	lycoris.differenceList = list.New()
	var specie = species{}
	var initialCapacity = int(float32(capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	specie.individualList = make([]Individual, initialCapacity)
	for i := 0; i < initialCapacity; i++ {
		specie.individualList[i] = *newIndividual(inputNum, outputNum)
	}
	lycoris.speciesList = append(lycoris.speciesList, specie)
	return lycoris
}

var wait sync.WaitGroup

func (lycoris *Lycoris) mate() {
	var specieLength = len(lycoris.speciesList)
	lycoris.bufferList = make([][]Individual, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(lycoris.speciesList[i].individualList)
		var mateLength = int(float32(individualLength) * mateOdds)
		lycoris.bufferList[i] = make([]Individual, mateLength)
		var part = mateLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go lycoris.mate_core(i, j*part, (j+1)*part)
		}
		go lycoris.mate_core(i, (cpuNum-1)*part, mateLength)
		wait.Wait()
	}
}

func (lycoris *Lycoris) mate_core(specieNum int, start int, end int) {
	var l = lycoris.speciesList[specieNum].individualList
	var length = len(l)
	for i := start; i < end; i++ {
		lycoris.bufferList[specieNum][i] = *mateIndividual(&(l[GetRandomInt(length)]), &(l[GetRandomInt(length)]))
	}
	wait.Done()
}

func (lycoris *Lycoris) mutate() {
	var specieLength = len(lycoris.speciesList)
	lycoris.bufferList = make([][]Individual, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(lycoris.speciesList[i].individualList)
		var mutateLength = int(float32(individualLength) * mutateOdds)
		lycoris.bufferList[i] = make([]Individual, mutateLength)
		var part = mutateLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go lycoris.mutate_core(i, j*part, (j+1)*part)
		}
		go lycoris.mutate_core(i, (cpuNum-1)*part, mutateLength)
		wait.Wait()
	}
}

func (lycoris *Lycoris) mutate_core(specieNum int, start int, end int) {
	var l = lycoris.speciesList[specieNum].individualList
	var length = len(l)
	for i := start; i < end; i++ {
		lycoris.bufferList[specieNum][i] = *mutateIndividual(&(l[GetRandomInt(length)]))
	}
	wait.Done()
}

func (lycoris *Lycoris) classify() {
	var specieLength = len(lycoris.speciesList)
	lycoris.tempDistance = make([][]int, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var bufferLength = len(lycoris.bufferList[i])
		lycoris.tempDistance[i] = make([]int, bufferLength)
		var part = bufferLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go lycoris.classify_core(i, j*part, (j+1)*part)
		}
		go lycoris.classify_core(i, (cpuNum-1)*part, bufferLength)
		wait.Wait()
	}
	lycoris.speciesList = append(lycoris.speciesList, species{})
	for k1, v1 := range lycoris.tempDistance {
		for k2, v2 := range v1 {
			if v2 != -1 {
				lycoris.speciesList[v2].individualList = append(lycoris.speciesList[v2].individualList, lycoris.bufferList[k1][k2])
			} else {
				lycoris.speciesList[specieLength].individualList = append(lycoris.speciesList[specieLength].individualList, lycoris.bufferList[k1][k2])
			}
		}
	}
	var tempSpeciesList []species
	for _, v := range lycoris.speciesList {
		if len(v.individualList) != 0 {
			tempSpeciesList = append(tempSpeciesList, v)
		}
	}
	lycoris.speciesList = tempSpeciesList
}

func (lycoris *Lycoris) classify_core(specieNum int, start int, end int) {
	var specieLength = len(lycoris.speciesList)
	for i := start; i < end; i++ {
		for j := 0; j < specieLength; j++ {
			var list = lycoris.speciesList[j].individualList
			var dis = distance(&(lycoris.bufferList[specieNum][i]), &(list[GetRandomInt(len(list))]))
			if dis <= distanceThreshold {
				lycoris.tempDistance[specieNum][i] = j
				break
			} else {
				lycoris.tempDistance[specieNum][i] = -1
			}
		}
	}
	wait.Done()
}

func (lycoris *Lycoris) forward() {
	var specieLength = len(lycoris.speciesList)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(lycoris.speciesList[i].individualList)
		var part = individualLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go lycoris.forward_core(i, j*part, (j+1)*part)
		}
		go lycoris.forward_core(i, (cpuNum-1)*part, individualLength)
		wait.Wait()
	}
}

func (lycoris *Lycoris) forward_core(specieNum int, start int, end int) {
	for i := start; i < end; i++ {
		lycoris.forwardFunc(&(lycoris.speciesList[specieNum].individualList[i]))
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

func (lycoris *Lycoris) autoParameter() {
	if checkFlag {
		var length = lycoris.differenceList.Len()
		var lastValue = lycoris.differenceList.Back().Value.(float32)
		var count = 0
		for e := lycoris.differenceList.Front(); e != nil; e = e.Next() {
			if lastValue >= e.Value.(float32) {
				count++
			}
		}
		if count < (length/2 + 1) { // Miss.
			lycoris.miss++
			lycoris.hit = 0
			if lycoris.miss == 2 {
				if lycoris.tock < 512 {
					lycoris.tock *= 2
				}
				lycoris.miss = 1
			}
			p1, p2, p3, p4, p5, p6, mateOdds, mutateOdds, mutateTime = p1_b, p2_b, p3_b, p4_b, p5_b, p6_b, mateOdds_b, mutateOdds_b, mutateTime_b
		} else { // Hit.
			lycoris.hit++
			lycoris.miss = 0
			if lycoris.hit == 2 {
				if lycoris.tock > 1 {
					lycoris.tock /= 2
				}
				lycoris.hit = 1
			}
		}
		checkFlag = false
	}

	if lycoris.tick == lycoris.tock {
		emergeArgs()
		checkFlag = true
		lycoris.tick = 0
	} else {
		lycoris.tick += 1
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

func (lycoris *Lycoris) chooseElite() {
	var totalLength = 0
	for _, v := range lycoris.speciesList {
		totalLength += len(v.individualList)
	}
	var sortList = (sort_fitness_array)(make([]sort_fitness, totalLength))
	var pointer = 0
	for k1, v1 := range lycoris.speciesList {
		for k2, v2 := range v1.individualList {
			sortList[pointer] = sort_fitness{v2.fitness, k1, k2}
			pointer++
		}
	}
	sort.Sort(sortList)
	var last = sortList[totalLength-1]
	var tempBest = lycoris.speciesList[last.specieNum].individualList[last.individualNum]

	var specieLength = len(lycoris.speciesList)
	var newSpecieList = make([]species, specieLength)
	var newLength = int(float32(lycoris.capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	for i := totalLength - 1; i > totalLength-newLength-1; i-- {
		var temp = sortList[i]
		newSpecieList[temp.specieNum].individualList = append(newSpecieList[temp.specieNum].individualList, lycoris.speciesList[temp.specieNum].individualList[temp.individualNum])
	}

	var tempSpeciesList []species
	for _, v := range newSpecieList {
		if len(v.individualList) != 0 {
			tempSpeciesList = append(tempSpeciesList, v)
		}
	}

	lycoris.speciesList = tempSpeciesList
	var difference = tempBest.fitness - lycoris.best.fitness
	lycoris.best = tempBest
	// len < 8
	if differenceListFlag {
		var length = lycoris.differenceList.Len()
		if length == 7 {
			differenceListFlag = false
		}
		lycoris.differenceList.PushBack(difference)
	} else { // len == 8
		lycoris.differenceList.Remove(lycoris.differenceList.Front())
		lycoris.differenceList.PushBack(difference)
	}
}

// Each time this function is called, the network runs forward one time.
func (lycoris *Lycoris) RunLycoris() {
	lycoris.mate()          // Mating.
	lycoris.classify()      // Computing distances of new individuals.
	lycoris.mutate()        // Mutating.
	lycoris.classify()      // Computing distances of new individuals.
	lycoris.forward()       // Forward calculation.
	lycoris.autoParameter() // Changing some parameters automatically.
	lycoris.chooseElite()   // Sorting and choosing some individuals with higher fitness.
}
