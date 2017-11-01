package LycorisNet

// The "innovationNum" is the cumulative number of connections
// and the "nodeSum" is that of nodes.
type individual struct {
	nodeMap       map[int]node
	nodeSlice     []int
	genomeMap     map[gen]ome
	inputNum      int
	outputNum     int
	innovationNum int
	nodeSum       int
	fitness       float64
}

func newIndividual(inputNum int, outputNum int) *individual {
	var in = &individual{inputNum: inputNum, outputNum: outputNum, nodeSum: 0, innovationNum: 0}
	initialize(in)
	return in
}

func initialize(in *individual) {
	var sumNode = in.inputNum + in.outputNum
	in.nodeMap = make(map[int]node, sumNode)
	in.nodeSlice = make([]int, sumNode)
	for i := 0; i < in.inputNum; i++ {
		var temp = in.nodeSum
		in.nodeMap[temp] = *newNode(temp, 0)
		in.nodeSlice[temp] = temp
		in.nodeSum++
	}
	for i := 0; i < in.outputNum; i++ {
		var temp = in.nodeSum
		in.nodeMap[temp] = *newNode(temp, 2)
		in.nodeSlice[temp] = temp
		in.nodeSum++
	}
	createGenes(in)
}

func createGenes(in *individual) {
	in.genomeMap = make(map[gen]ome, in.inputNum*in.outputNum)
	for i := in.inputNum; i < in.nodeSum; i++ {
		for j := 0; j < in.inputNum; j++ {
			var g = gen{j, i}
			var o = ome{random(), true, in.innovationNum}
			in.genomeMap[g] = o
			in.nodeMap[i].genomeMap[g] = o
			in.innovationNum++
		}
	}
}

func (in *individual) setInput(input []float64) {
	for i := 0; i < in.inputNum; i++ {
		var temp = in.nodeMap[i]
		temp.value = input[i]
		in.nodeMap[i] = temp
	}
}

func (in *individual) getOutput() []float64 {
	var output = make([]float64, in.outputNum)
	var pointer = 0
	for i := in.inputNum; i < len(in.nodeSlice); i++ {
		var temp = in.nodeMap[in.nodeSlice[i]]
		if temp.nodeType == 2 {
			output[pointer] = temp.value
			pointer++
		}
	}
	return output
}

func (in *individual) forward() {
	for i := in.inputNum; i < len(in.nodeSlice); i++ {
		var index = in.nodeSlice[i]
		var n = in.nodeMap[index]
		var f float64 = 0
		for g, o := range n.genomeMap {
			if o.isEnable {
				f += in.nodeMap[g.in].value * o.weight
			}
		}
		n.value = activateFunc(f)
		in.nodeMap[index] = n
	}
}

func (in *individual) clone() *individual {
	var duplicate = &individual{inputNum: in.inputNum, outputNum: in.outputNum, innovationNum: in.innovationNum, nodeSum: in.nodeSum}
	duplicate.nodeMap = make(map[int]node, len(in.nodeMap))
	for k, v := range in.nodeMap {
		duplicate.nodeMap[k] = *v.clone()
	}
	duplicate.genomeMap = make(map[gen]ome, len(in.genomeMap))
	for k, v := range in.genomeMap {
		duplicate.genomeMap[k] = v
	}
	duplicate.nodeSlice = make([]int, len(in.nodeSlice))
	copy(duplicate.nodeSlice, in.nodeSlice)
	return duplicate
}
