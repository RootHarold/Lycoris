package LycorisNet

import "container/list"

type genomes struct {
	value list.List
}

type Individual struct {
	NodeList      list.List
	GenomeList    list.List
	NodeGenomeMap map[Node]genomes
	InputNum      int
	OutputNum     int
	InnovationNum int
	NodeNum       int
	Fitness       float64
}

func NewIndividual(InputNum int, OutputNum int) *Individual {
	var individual *Individual = &Individual{InputNum: InputNum, OutputNum: OutputNum, NodeNum: 0, InnovationNum: 0}
	initialize(individual)
	return individual
}

func initialize(individual *Individual) {
	for i := 0; i < individual.InputNum; i++ {

	}
	for i := 0; i < individual.OutputNum; i++ {

	}
	createGenes(individual)
}

func createGenes(individual *Individual) {

}

func (individual *Individual) SetInput(input []float64) {

}

func (individual *Individual) GetOutput() {

}

func (individual *Individual) Forward() {

}
