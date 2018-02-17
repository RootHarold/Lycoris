package LycorisNet

import (
	"runtime"
	"sync"
)

type Lycoris struct {
	speciesList []species
	forwardFunc func(in *individual)
	best        individual
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
	// TODO
}

func mate_core(lycoris *Lycoris, specieNum int, mateTime int) {
	// TODO
}

func (lycoris *Lycoris) mutate() {
	// TODO
}

func (lycoris *Lycoris) classify([] individual) {
	// TODO
}

func (lycoris *Lycoris) forward() {

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
	lycoris.mutate()
	lycoris.forward()
	lycoris.autoParameter()
	lycoris.chooseElite()
}
