package LycorisNet

import (
	"container/list"
)

type Individual struct {
	NodeList      list.List
	GenomeMap     map[Gen]Ome
	InputNum      int
	OutputNum     int
	InnovationNum int
	NodeNum       int
	Fitness       float64
}

func NewIndividual(InputNum int, OutputNum int) *Individual {
	var individual *Individual = &Individual{InputNum: InputNum, OutputNum: OutputNum, NodeNum: 0, InnovationNum: 0}
	individual.GenomeMap = make(map[Gen]Ome)
	initialize(individual)
	return individual
}

func initialize(individual *Individual) {
	for i := 0; i < individual.InputNum; i++ {
		individual.NodeList.PushBack(*NewNode(individual.NodeNum, 0))
		individual.NodeNum++
	}
	for i := 0; i < individual.OutputNum; i++ {
		individual.NodeList.PushBack(*NewNode(individual.NodeNum, 2))
		individual.NodeNum++
	}
	createGenes(individual)
}

func createGenes(individual *Individual) {
	
}

func (individual *Individual) SetInput(input []float64) {
	var node = individual.NodeList.Front()
	for i := 0; i < individual.InputNum; i++ {
		var temp = node.Value.(Node)
		temp.Value = input[i]
		node = node.Next()
	}
}

func (individual *Individual) GetOutput() []float64 {
	var output = make([]float64, individual.OutputNum)
	var nodeList = individual.NodeList
	var len = nodeList.Len()
	var node = nodeList.Front()
	var pointer = 0
	for i := 0; i < len; i++ {
		if i < individual.InputNum {
			node = node.Next()
		} else {
			var temp = node.Value.(Node)
			if temp.NodeType == 2 {
				output[pointer] = temp.Value
				pointer++
			}
		}
	}
	return output
}

func (individual *Individual) Forward() {

}
