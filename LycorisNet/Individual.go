package LycorisNet

// The "innovationNum" is the cumulative number of connections
// and the "nodeSum" is that of nodes. High fitness means the
// individual is well adapted to the environment. The "nodeMap"
// and the "nodeSlice" store nodes.
type Individual struct {
	nodeMap       map[int]node
	nodeSlice     []int
	InputNum      int
	OutputNum     int
	innovationNum int
	nodeSum       int
	Fitness       float32
}

// Create a new individual.
func newIndividual(inputNum int, outputNum int) *Individual {
	var in = &Individual{InputNum: inputNum, OutputNum: outputNum, nodeSum: 0, innovationNum: 0}
	initialize(in)
	return in
}

// Initialize a individual.
func initialize(in *Individual) {
	var sumNode = in.InputNum + in.OutputNum
	in.nodeMap = make(map[int]node, sumNode)
	in.nodeSlice = make([]int, sumNode)
	for i := 0; i < in.InputNum; i++ {
		var temp = in.nodeSum
		in.nodeMap[temp] = *newNode(temp, 0)
		in.nodeSlice[temp] = temp
		in.nodeSum++
	}
	for i := 0; i < in.OutputNum; i++ {
		var temp = in.nodeSum
		in.nodeMap[temp] = *newNode(temp, 2)
		in.nodeSlice[temp] = temp
		in.nodeSum++
	}
}

// Set input array.
func (in *Individual) SetInput(input []float32) {
	for i := 0; i < in.InputNum; i++ {
		var temp = in.nodeMap[i]
		temp.value = input[i]
		in.nodeMap[i] = temp
	}
}

// Get output array.
func (in *Individual) GetOutput() []float32 {
	var output = make([]float32, in.OutputNum)
	var pointer = 0
	for i := in.InputNum; i < len(in.nodeSlice); i++ {
		var temp = in.nodeMap[in.nodeSlice[i]]
		if temp.nodeType == 2 {
			output[pointer] = temp.value
			pointer++
		}
	}
	return output
}

// The odds of cleaning in "Forward()".
var cleanOdds float32 = 0.008

// Forward calculation of the individual.
func (in *Individual) Forward() {
	var clean = make(map[int]bool)
	for i := in.InputNum; i < len(in.nodeSlice); i++ {
		var index = in.nodeSlice[i]
		var n = in.nodeMap[index]
		var f float32 = 0
		if len(n.genomeMap) == 0 {
			if n.nodeType == 1 {
				clean[index] = true
			}
		} else {
			for g, o := range n.genomeMap {
				if o.isEnable {
					f += in.nodeMap[g.in].value * o.weight
				}
			}
			n.value = activateFunc(f + n.bias)
			in.nodeMap[index] = n
		}
	}

	// Clean empty nodes according to a certain chance (cleanOdds).
	if GetRandomFloat32() < cleanOdds && len(clean) != 0 {
		for k := range clean {
			delete(in.nodeMap, k)
		}

		var tempSlice = make([]int, len(in.nodeSlice)-len(clean))
		var count = 0
		for _, v := range in.nodeSlice {
			var _, ok = clean[v]
			if !ok {
				tempSlice[count] = v
				count++
			}
		}
		in.nodeSlice = tempSlice
	}
}

// Deep clone of individual.
func (in *Individual) clone() *Individual {
	var duplicate = &Individual{InputNum: in.InputNum, OutputNum: in.OutputNum, innovationNum: in.innovationNum, nodeSum: in.nodeSum}
	duplicate.nodeMap = make(map[int]node, len(in.nodeMap))
	for k, v := range in.nodeMap {
		duplicate.nodeMap[k] = *v.clone()
	}
	duplicate.nodeSlice = make([]int, len(in.nodeSlice))
	copy(duplicate.nodeSlice, in.nodeSlice)
	return duplicate
}
