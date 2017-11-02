package LycorisNet

// The "innovationNum" is the cumulative number of connections
// and the "nodeSum" is that of nodes. High fitness means the
// individual is well adapted to the environment. The "nodeMap"
// and the "nodeSlice" store nodes.
type individual struct {
	nodeMap       map[int]node
	nodeSlice     []int
	inputNum      int
	outputNum     int
	innovationNum int
	nodeSum       int
	fitness       float64
}

// Create a new individual.
func newIndividual(inputNum int, outputNum int) *individual {
	var in = &individual{inputNum: inputNum, outputNum: outputNum, nodeSum: 0, innovationNum: 0}
	initialize(in)
	return in
}

// Initialize a individual.
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

// Create genes of a new individual.
func createGenes(in *individual) {
	for i := in.inputNum; i < in.nodeSum; i++ {
		for j := 0; j < in.inputNum; j++ {
			var g = gen{j, i}
			var o = ome{random(), true, in.innovationNum}
			in.nodeMap[i].genomeMap[g] = o
			in.innovationNum++
		}
	}
}


// Set input array.
func (in *individual) setInput(input []float64) {
	for i := 0; i < in.inputNum; i++ {
		var temp = in.nodeMap[i]
		temp.value = input[i]
		in.nodeMap[i] = temp
	}
}

// Get output array.
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

// Forward calculation of the individual.
func (in *individual) forward() {
	var clean []int
	for i := in.inputNum; i < len(in.nodeSlice); i++ {
		var index = in.nodeSlice[i]
		var n = in.nodeMap[index]
		var f float64 = 0
		if len(n.genomeMap) == 0 {
			if n.nodeType == 1 {
				clean = append(clean, index)
				delete(in.nodeMap, index)
			}
		} else {
			for g, o := range n.genomeMap {
				if o.isEnable {
					f += in.nodeMap[g.in].value * o.weight
				}
			}
			n.value = activateFunc(f)
			in.nodeMap[index] = n
		}
	}
	if len(clean) != 0 {
		var tempSlice = make([]int, len(in.nodeSlice)-len(clean))
		var count = 0
		for _, v1 := range in.nodeSlice {
			var flag = true
			for _, v2 := range clean {
				if v1 == v2 {
					flag = false
					break
				}
			}
			if flag {
				tempSlice[count] = v1
				count++
			}
		}
		in.nodeSlice = tempSlice
	}
}

// Deep clone of individual.
func (in *individual) clone() *individual {
	var duplicate = &individual{inputNum: in.inputNum, outputNum: in.outputNum, innovationNum: in.innovationNum, nodeSum: in.nodeSum}
	duplicate.nodeMap = make(map[int]node, len(in.nodeMap))
	for k, v := range in.nodeMap {
		duplicate.nodeMap[k] = *v.clone()
	}
	duplicate.nodeSlice = make([]int, len(in.nodeSlice))
	copy(duplicate.nodeSlice, in.nodeSlice)
	return duplicate
}
