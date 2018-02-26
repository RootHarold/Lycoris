package LycorisNet

import (
	"runtime"
	"container/list"
)

// The number of logical cpu.
var cpuNum = runtime.NumCPU()

type utils struct {
	c1                float32 // Arguments in distance(...).
	c2                float32 // Arguments in distance(...).
	p1                float32 // Add the new node between a connection.
	p2                float32 // Delete a node.
	p3                float32 // Add a new connection between two nodes.
	p4                float32 // Delete a connection.
	p5                float32 // Just create a new empty node (without any genomes).
	p6                float32 // Mutate the bias.
	mutateTime        int     // The number of mutations.
	mateOdds          float32 // The initial odds of mating.
	mutateOdds        float32 // The initial odds of mutating.
	maxMutateTime     int     // The maximum number of mutating.
	distanceThreshold float32 // The threshold of the distance between two individuals.
	weightA           float32
	weightB           float32
	biasA             float32
	biasB             float32
	activateFuncName  string
	activateFun       func(f float32) float32
}

func newUtils() *utils {
	return &(utils{
		c1:                1.0,
		c2:                0.4,
		p1:                0.1,
		p2:                0.2,
		p3:                0.2,
		p4:                0.2,
		p5:                0.1,
		p6:                0.2,
		mutateTime:        1,
		mateOdds:          1,
		mutateOdds:        1,
		maxMutateTime:     10,
		distanceThreshold: 20,
		weightA:           -1,
		weightB:           1,
		biasA:             -1,
		biasB:             1,
		activateFuncName:  "sigmoid",
		activateFun:       Sigmoid,
	})
}

// Set the threshold of the distance between two individuals.
func (u *utils) SetDistanceThreshold(threshold float32) {
	u.distanceThreshold = threshold
}

//Set "c1" and "c2".
func (u *utils) SetDistanceArgs(a float32, b float32) {
	u.c1, u.c2 = a, b
}

// Set the maximum number of mutating.
func (u *utils) SetMaxMutateTime(num int) {
	u.maxMutateTime = num
}

// This is for initializing weight.
func (u *utils) weightRandom() float32 {
	return LycorisRandomFloat32()*(u.weightB-u.weightA) + u.weightA
}

// Set the weightRandom()'s range.
func (u *utils) SetWeigthRandom(a float32, b float32) {
	u.weightA, u.weightB = a, b
}

// This is for initializing bias.
func (u *utils) biasRandom() float32 {
	return LycorisRandomFloat32()*(u.biasB-u.biasA) + u.biasA
}

// Set the biasRandom()'s range.
func (u *utils) SetBiasRandom(a float32, b float32) {
	u.biasA, u.biasB = a, b
}

// Set the activate function.
func (u *utils) SetActivateFunc(function string) {
	u.activateFuncName = function
	if function == "sigmoid" {
		u.activateFun = Sigmoid
	} else if function == "tanh" {
		u.activateFun = Tanh
	} else if function == "relu" {
		u.activateFun = ReLU
	} else {
		panic("Wrong activate function!")
	}
}

// The distance between two different individuals.
func (u *utils) distance(in1 *Individual, in2 *Individual) float32 {
	var d float32
	var DE = 0
	var w1, i1 = sort1(in1)
	var w2, i2 = sort1(in2)
	var N = 0
	var len1 = len(*w1)
	var len2 = len(*w2)
	if len1 > len2 {
		N = len1
	} else {
		N = len2
	}
	if N == 0 {
		return 0
	}
	var W float32 = 0
	var countW = 0
	var point1 = 0
	var point2 = 0
	for true {
		if point1 == len1 || point2 == len2 {
			break
		}
		if (*i1)[point1] == (*i2)[point2] {
			W += LycorisAbs((*w1)[point1] - (*w2)[point2])
			countW++
			point1++
			point2++
		} else if (*i1)[point1] > (*i2)[point2] {
			DE++
			point2++
		} else {
			DE++
			point1++
		}
	}
	DE += len1 + len2 - point1 - point2
	d = (u.c1*float32(DE))/float32(N) + (u.c2*float32(W))/float32(countW)
	return d
}

// Mating two different individuals.
func (u *utils) mateIndividual(in1 *Individual, in2 *Individual) *Individual {
	// Emerge a newborn one.
	var offspring = Individual{InputNum: in1.InputNum, OutputNum: in1.OutputNum}
	offspring.nodeMap = make(map[int]node)

	// Let the better individual be "in2".
	if in1.Fitness > in2.Fitness {
		var temp = in1
		in1 = in2
		in2 = temp
	}

	// Mate nodes.
	var tempMap = make(map[int]bool)
	var duplicateNodes = make(map[int]bool)
	for k := range in1.nodeMap {
		tempMap[k] = true
		duplicateNodes[k] = false
	}
	for k := range in2.nodeMap {
		_, ok := tempMap[k]
		if ok {
			duplicateNodes[k] = true
		} else {
			tempMap[k] = true
			duplicateNodes[k] = false
		}
	}
	var tempSlice = make([]int, len(tempMap))
	var tempCount = 0
	for k := range tempMap {
		tempSlice[tempCount] = k
		tempCount++
	}

	// Mate the bias of nodes. And the strategy is choosing bias
	// between parents randomly.
	var basicNodeSum = in1.InputNum + in1.OutputNum
	for _, v := range tempSlice {
		var n node
		if v < offspring.InputNum {
			n = *newNode(v, 0)
		} else if v >= offspring.InputNum && v < basicNodeSum {
			n = *newNode(v, 2)
			if duplicateNodes[v] {
				if LycorisRandomFloat32() < 0.5 {
					n.bias = in1.nodeMap[v].bias
				} else {
					n.bias = in2.nodeMap[v].bias
				}
			}
		} else {
			n = *newNode(v, 1)
			if duplicateNodes[v] {
				if LycorisRandomFloat32() < 0.5 {
					n.bias = in1.nodeMap[v].bias
				} else {
					n.bias = in2.nodeMap[v].bias
				}
			}
		}
		offspring.nodeMap[v] = n
	}

	// Mate genomes.
	var g1, o1 = sort2(in1)
	var g2, o2 = sort2(in2)
	var point1 = 0
	var point2 = 0
	var len1 = len(*g1)
	var len2 = len(*g2)
	for true {
		if point1 == len1 || point2 == len2 {
			break
		}
		if (*o1)[point1].innovationNum == (*o2)[point2].innovationNum {
			if LycorisRandomFloat32() < 0.5 {
				offspring.nodeMap[(*g1)[point1].out].genomeMap[(*g1)[point1]] = (*o1)[point1]
			} else {
				offspring.nodeMap[(*g2)[point2].out].genomeMap[(*g2)[point2]] = (*o2)[point2]
			}
			point1++
			point2++
		} else if (*o1)[point1].innovationNum > (*o2)[point2].innovationNum {
			offspring.nodeMap[(*g2)[point2].out].genomeMap[(*g2)[point2]] = (*o2)[point2]
			point2++
		} else {
			offspring.nodeMap[(*g1)[point1].out].genomeMap[(*g1)[point1]] = (*o1)[point1]
			point1++
		}
	}
	for i := point1; i < len1; i++ {
		offspring.nodeMap[(*g1)[point1].out].genomeMap[(*g1)[point1]] = (*o1)[point1]
	}
	for i := point2; i < len2; i++ {
		offspring.nodeMap[(*g2)[point2].out].genomeMap[(*g2)[point2]] = (*o2)[point2]
	}

	// Make the nodes in a reasonable order of calculation.
	var inDegree = make(map[int]int, len(tempSlice))
	for k, v := range offspring.nodeMap {
		inDegree[k] = len(v.genomeMap)
	}
	var next = list.New()
	for k, v := range inDegree {
		if v == 0 {
			next.PushBack(k)
			delete(inDegree, k)
		}
	}
	var slicePointer = 0
	for true { // Topological Order
		if next.Len() == 0 {
			break
		}
		head := next.Front()
		headValue := head.Value.(int)
		tempSlice[slicePointer] = headValue
		slicePointer++
		next.Remove(head)
		for _, v := range offspring.nodeMap {
			for k := range v.genomeMap {
				if k.in == headValue {
					outNum := v.nodeNum
					if inDegree[outNum] == 1 {
						delete(inDegree, outNum)
						next.PushBack(outNum)
					} else {
						inDegree[outNum] = inDegree[outNum] - 1
					}
					break
				}
			}
		}
	}
	for k := range inDegree {
		tempSlice[slicePointer] = k
		slicePointer++
	}

	// Save nodes in a slice.
	offspring.nodeSlice = make([]int, len(tempSlice))
	for i := 0; i < in1.InputNum; i++ {
		offspring.nodeSlice[i] = i
	}
	var point = in1.InputNum
	for _, v := range tempSlice {
		if v >= in1.InputNum {
			offspring.nodeSlice[point] = v
			point++
		}
	}

	// Choose the nodeSum of offspring.
	if in1.nodeSum > in2.nodeSum {
		offspring.nodeSum = in1.nodeSum
	} else {
		offspring.nodeSum = in2.nodeSum
	}

	// Choose the innovationNum of offspring.
	if in1.innovationNum > in2.innovationNum {
		offspring.innovationNum = in1.innovationNum
	} else {
		offspring.innovationNum = in2.innovationNum
	}

	return &offspring
}

// Mutating the individual.
func (u *utils) mutateIndividual(in *Individual) *Individual {
	// Clone the individual.
	var offspring = in.clone()

	// This process can be repeated many times.
	for z := 0; z < u.mutateTime; z++ {
		var ran = LycorisRandomFloat32()

		if ran < u.p1 { // p1
			// Add the new node between a connection.
			var genOld gen
			var omeOld ome
			var nodeOld node
			var notNull = false
			// This is very dependent on the random mechanism in golang map.
			for _, v1 := range offspring.nodeMap {
				nodeOld = v1
				for k2, v2 := range v1.genomeMap {
					genOld = k2
					omeOld = v2
					notNull = true
					break
				}
				break
			}
			// If there is a suitable connection.
			if notNull {
				omeOld.isEnable = false
				var n = *newNode(offspring.nodeSum, 1)
				offspring.nodeSum++
				var g1, o1 = newGenome(genOld.in, n.nodeNum, 1.0, true, offspring.innovationNum)
				offspring.innovationNum++
				var g2, o2 = newGenome(n.nodeNum, genOld.out, omeOld.weight, true, offspring.innovationNum)
				offspring.innovationNum++
				n.genomeMap[*g1] = *o1
				nodeOld.genomeMap[genOld] = omeOld
				nodeOld.genomeMap[*g2] = *o2
				offspring.nodeMap[genOld.out] = nodeOld
				offspring.nodeMap[n.nodeNum] = n

				var length = len(offspring.nodeSlice)
				var newSlice = make([]int, length+1)
				var count = 0
				for i := 0; i < length; i++ {
					if offspring.nodeSlice[i] == genOld.out {
						newSlice[count] = n.nodeNum
						count++
					}
					newSlice[count] = offspring.nodeSlice[i]
					count++
				}
				offspring.nodeSlice = newSlice
			}
		} else if ran >= u.p1 && ran < u.p1+u.p2 { // p2
			// Delete a node.
			var length = len(offspring.nodeSlice)
			var index = LycorisRandomInt(length)
			var sliceIndex = offspring.nodeSlice[index]
			if offspring.nodeMap[sliceIndex].nodeType == 1 {
				for i := index + 1; i < length; i++ {
					var outputNum = offspring.nodeSlice[i]
					var n = offspring.nodeMap[outputNum]
					delete(n.genomeMap, gen{sliceIndex, outputNum})
					offspring.nodeMap[outputNum] = n
				}
				delete(offspring.nodeMap, sliceIndex)
				offspring.nodeSlice = append(offspring.nodeSlice[:index], offspring.nodeSlice[index+1:]...)
			}
		} else if ran >= u.p1+u.p2 && ran < u.p1+u.p2+u.p3 { // p3
			// Add a new connection between two nodes.
			var length = len(offspring.nodeSlice)
			var index1 = LycorisRandomInt(length)
			var index2 = index1 + LycorisRandomInt(length-index1)
			if index1 != index2 {
				var inputNum = offspring.nodeSlice[index1]
				var outputNum = offspring.nodeSlice[index2]
				var inputNode = offspring.nodeMap[inputNum]
				var outputNode = offspring.nodeMap[outputNum]
				if !((inputNode.nodeType == 0 && outputNode.nodeType == 0) || (inputNode.nodeType == 2 && outputNode.nodeType == 2)) {
					var g = gen{inputNum, outputNum}
					var _, ok = outputNode.genomeMap[g]
					if !ok {
						var o = ome{u.weightRandom(), true, offspring.innovationNum}
						offspring.innovationNum++
						outputNode.genomeMap[g] = o
						offspring.nodeMap[outputNum] = outputNode
					}
				}
			}
		} else if ran >= u.p1+u.p2+u.p3 && ran < u.p1+u.p2+u.p3+u.p4 { // p4
			// Delete a connection.
			var g gen
			var n node
			var notNull = false
			// This is very dependent on the random mechanism in golang map.
			for _, v := range offspring.nodeMap {
				n = v
				for k := range v.genomeMap {
					g = k
					notNull = true
					break
				}
				break
			}
			if notNull {
				delete(n.genomeMap, g)
				offspring.nodeMap[g.out] = n
			}
		} else if ran >= u.p1+u.p2+u.p3+u.p4 && ran < u.p1+u.p2+u.p3+u.p4+u.p5 { // p5
			// Just create a new empty node (without any genomes).
			var n = *newNode(offspring.nodeSum, 1)
			offspring.nodeSum++
			offspring.nodeMap[n.nodeNum] = n
			var index = LycorisRandomInt(len(offspring.nodeSlice)-offspring.InputNum) + offspring.InputNum
			rear := append([]int{n.nodeNum}, offspring.nodeSlice[index:]...)
			offspring.nodeSlice = append(offspring.nodeSlice[:index], rear...)
		} else { // p6
			// Mutate the bias.
			var index = LycorisRandomInt(len(offspring.nodeSlice) - offspring.InputNum)
			index += offspring.InputNum
			var n = offspring.nodeMap[offspring.nodeSlice[index]]
			n.bias = u.biasRandom()
			offspring.nodeMap[n.nodeNum] = n
		}
	}

	return offspring
}
