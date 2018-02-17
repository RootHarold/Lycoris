package LycorisNet

import (
	"runtime"
	"sync"
)

type Lycoris struct {
	speciesList  []species
	forwardFunc  func(in *individual)
	best         individual
	bufferList   [][]individual
	tempDistance [][]int
}

func (lycoris *Lycoris) setForwardFunc(f func(in *individual)) {
	lycoris.forwardFunc = f
}

func newLycoris(capacity int, inputNum int, outputNum int) *Lycoris {
	runtime.GOMAXPROCS(runtime.NumCPU())
	go randFloat32() // Init the random number generator.

	var lycoris = &Lycoris{}
	var specie = species{}
	var initialCapacity = int(float32(capacity) / ((1 + mateOdds) * (1 + mutateOdds)))
	specie.individualList = make([]individual, initialCapacity)
	for i := 0; i < initialCapacity; i++ {
		specie.individualList[i] = *newIndividual(inputNum, outputNum)
	}
	lycoris.speciesList = append(lycoris.speciesList, specie)
	return lycoris
}

var wait sync.WaitGroup

func (lycoris *Lycoris) mate() {
	var specieLength = len(lycoris.speciesList)
	lycoris.bufferList = make([][]individual, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(lycoris.speciesList[i].individualList)
		var mateLength = int(float32(individualLength) * mateOdds)
		lycoris.bufferList[i] = make([]individual, mateLength)
		var part = mateLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go lycoris.mate_core(i, j*part, (j+1)*part)
		}
		go lycoris.mate_core(i, (cpuNum-1)*part, mateLength)
		wait.Wait()
	}
}

func (lycoris *Lycoris) mate_core(specieNum int, start int, end int) {
	var list = lycoris.speciesList[specieNum].individualList
	var length = len(list)
	for i := start; i < end; i++ {
		lycoris.bufferList[specieNum][i] = *mateIndividual(&(list[getInt(length)]), &(list[getInt(length)]))
	}
	wait.Done()
}

func (lycoris *Lycoris) mutate() {
	var specieLength = len(lycoris.speciesList)
	lycoris.bufferList = make([][]individual, specieLength)
	for i := 0; i < specieLength; i++ {
		wait.Add(cpuNum)
		var individualLength = len(lycoris.speciesList[i].individualList)
		var mutateLength = int(float32(individualLength) * mutateOdds)
		lycoris.bufferList[i] = make([]individual, mutateLength)
		var part = mutateLength / cpuNum
		for j := 0; j < cpuNum-1; j++ {
			go lycoris.mutate_core(i, j*part, (j+1)*part)
		}
		go lycoris.mutate_core(i, (cpuNum-1)*part, mutateLength)
		wait.Wait()
	}
}

func (lycoris *Lycoris) mutate_core(specieNum int, start int, end int) {
	var list = lycoris.speciesList[specieNum].individualList
	var length = len(list)
	for i := start; i < end; i++ {
		lycoris.bufferList[specieNum][i] = *mutateIndividual(&(list[getInt(length)]))
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
			var dis = distance(&(lycoris.bufferList[specieNum][i]), &(list[getInt(len(list))]))
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
		wait.Done()
	}
}

func (lycoris *Lycoris) forward_core(specieNum int, start int, end int) {
	for i := start; i < end; i++ {
		lycoris.forwardFunc(&(lycoris.speciesList[specieNum].individualList[i]))
	}
	wait.Done()
}

func (lycoris *Lycoris) autoParameter() {
	// TODO
}

type sort_fitness struct {
	fitness       float32
	specieNum     int
	individualNum int
}

type sort_fitness_array [] sort_fitness

func (f sort_fitness_array) Len() int {
	return len(f)
}

func (f sort_fitness_array) Less(i, j int) bool {
	return f[i].fitness < f[j].fitness
}

func (f sort_fitness_array) Swap(i, j int) {
	f[i], f[j] = f[j], f[i]
}

func (lycoris *Lycoris) chooseElite() {
	// TODO
}

func (lycoris *Lycoris) runLycoris() {
	lycoris.mate()
	lycoris.classify()
	lycoris.mutate()
	lycoris.classify()
	lycoris.forward()
	lycoris.autoParameter()
	lycoris.chooseElite()
}
