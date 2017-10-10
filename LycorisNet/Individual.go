package LycorisNet

type Individual struct {
	NodeList      []Node
	GenomeList    []Genome
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
		individual.NodeList = append(individual.NodeList, *NewNode(individual.NodeNum, 0))
		individual.NodeNum++
	}
	for i := 0; i < individual.OutputNum; i++ {
		individual.NodeList = append(individual.NodeList, *NewNode(individual.NodeNum, 2))
		individual.NodeNum++
	}
	createGenes(individual)
}

func createGenes(individual *Individual) {

}

func (individual *Individual) SetInput() {

}

func (individual *Individual) SetOutput() {

}

func (individual *Individual) Forward() {

}
