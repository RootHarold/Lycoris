package LycorisNet

import (
	"time"
	"math/rand"
	"math"
)

// It's for the function "reLU(...)".
var leak = 0.01
// Random number seed.
var r = rand.New(rand.NewSource(time.Now().UnixNano()))
// Arguments for the algorithm.
var c1 = 1.0
var c2 = 0.4
// Used in "mateIndividual(...)"
var breakTime = 12
// These change the mutation probability.
var p1 = 0.15
var p2 = 0.35
var p3 = 0.15
// The number of mutations.
var mutateTime = 1

// This is for whole algorithm.
func random() float64 {
	return r.Float64()
}

// The activation function.
func activateFunc(f float64) float64 {
	return reLU(f)
}

// ReLU.
func reLU(f float64) float64 {
	if f > 0 {
		return f
	} else {
		return f * leak
	}
}

// Sigmoid.
func sigmoid(f float64) float64 {
	return 1 / (1 + math.Exp(0-f))
}

// Tanh.
func tanh(f float64) float64 {
	return (math.Exp(f) - math.Exp(-f)) / (math.Exp(f) + math.Exp(-f))
}

// Used in "distance(...)".
func sort1(in *individual) (*[]float64, *[]int) {
	var temp1 = make([]bool, in.innovationNum)
	var temp2 = make([]float64, in.innovationNum)
	var temp3 = make([]int, in.innovationNum)

	var length = 0
	for _, v1 := range in.nodeMap {
		length += len(v1.genomeMap)
		for _, v2 := range v1.genomeMap {
			num := v2.innovationNum
			temp1[num] = true
			temp2[num] = v2.weight
			temp3[num] = num
		}
	}

	var result1 = make([]float64, length)
	var result2 = make([]int, length)
	var count = 0
	for k, v := range temp1 {
		if v {
			result1[count] = temp2[k]
			result2[count] = temp3[k]
			count++
		}
	}
	return &result1, &result2
}

// Used in "mateIndividual(...)".
func sort2(in *individual) (*[]gen, *[]ome) {
	var temp1 = make([]bool, in.innovationNum)
	var temp2 = make([]gen, in.innovationNum)
	var temp3 = make([]ome, in.innovationNum)

	var length = 0
	for _, v1 := range in.nodeMap {
		length += len(v1.genomeMap)
		for k, v2 := range v1.genomeMap {
			num := v2.innovationNum
			temp1[num] = true
			temp2[num] = k
			temp3[num] = v2
		}
	}

	var result1 = make([]gen, length)
	var result2 = make([]ome, length)
	var count = 0
	for k, v := range temp1 {
		if v {
			result1[count] = temp2[k]
			result2[count] = temp3[k]
			count++
		}
	}
	return &result1, &result2
}

// Euclidean distance of two slices.
func error(output []float64, desire []float64) float64 {
	var f float64 = 0
	for i := 0; i < len(output); i++ {
		f += math.Pow(desire[i]-output[i], 2)
	}
	return f
}

// The distance between two different individuals.
func distance(in1 *individual, in2 *individual) float64 {
	var d float64
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
	var W float64 = 0
	var countW = 0
	var point1 = 0
	var point2 = 0
	for true {
		if point1 == len1 || point2 == len2 {
			break
		}
		if (*i1)[point1] == (*i2)[point2] {
			W += math.Abs((*w1)[point1] - (*w2)[point1])
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
	d = (c1*float64(DE))/float64(N) + (c2*float64(W))/float64(countW)
	return d
}

// Mating two different individuals.
func mateIndividual(in1 *individual, in2 *individual) *individual {
	var offspring = individual{inputNum: in1.inputNum, outputNum: in1.outputNum}
	offspring.nodeMap = make(map[int]node)
	if in1.fitness > in2.fitness {
		var temp = in1
		in1 = in2
		in2 = temp
	}

	var tempMap = make(map[int]int)
	for _, v := range in1.nodeMap {
		tempMap[v.nodeNum] = -1
	}
	for _, v := range in2.nodeMap {
		tempMap[v.nodeNum] = -1
	}
	var tempSlice = make([]int, len(tempMap))
	var tempCount = 0
	for k := range tempMap {
		tempSlice[tempCount] = k
		tempMap[k] = tempCount
		tempCount++
	}
	var bt = 0
	for true {
		bt++
		var flag = true
		for _, v := range offspring.nodeMap {
			for k := range v.genomeMap {
				var indexIn = tempMap[k.in]
				var indexOut = tempMap[k.out]
				if indexIn > indexOut {
					flag = false
					tempSlice[indexIn] = k.out
					tempSlice[indexOut] = k.in
					tempMap[k.out] = indexIn
					tempMap[k.in] = indexOut
				}
			}
		}
		if flag || bt == breakTime {
			break
		}
	}

	var lenTempSlice = len(tempSlice)
	offspring.nodeSlice = make([]int, lenTempSlice)
	for i := 0; i < in1.inputNum; i++ {
		offspring.nodeSlice[i] = i
	}
	var temp = in1.inputNum + in1.outputNum
	for i := in1.inputNum; i < temp; i++ {
		offspring.nodeSlice[lenTempSlice-temp+i] = i
	}
	var point = temp
	for _, v := range tempSlice {
		if v >= temp {
			offspring.nodeSlice[point] = v
			point++
		}
	}

	for _, v := range offspring.nodeSlice {
		var n node
		if v < offspring.inputNum {
			n = *newNode(v, 0)
		} else if v >= offspring.inputNum && v < temp {
			n = *newNode(v, 2)
		} else {
			n = *newNode(v, 1)
		}
		offspring.nodeMap[v] = n
	}

	if in1.nodeSum > in2.nodeSum {
		offspring.nodeSum = in1.nodeSum
	} else {
		offspring.nodeSum = in2.nodeSum
	}

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
			if r.Float64() < 0.5 {
				offspring.nodeMap[(*g1)[point1].out].genomeMap[(*g1)[point1]] = (*o1)[point1]
			} else {
				offspring.nodeMap[(*g2)[point2].out].genomeMap[(*g2)[point2]] = (*o2)[point2]
			}
			point1++
			point2++
		} else if (*o1)[point1].innovationNum > (*o2)[point2].innovationNum {
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

	var temp1 = (*o1)[point1-1].innovationNum
	var temp2 = (*o2)[point2-1].innovationNum
	if temp1 > temp2 {
		offspring.innovationNum = temp1 + 1
	} else {
		offspring.innovationNum = temp2 + 1
	}

	return &offspring
}

// Mutating the individual.
func mutateIndividual(in *individual) *individual {
	var offspring = in.clone()
	for z := 0; z < mutateTime; z++ {
		var ran = r.Float64()
		if ran < p1 {
			var genOld gen
			var omeOld ome
			var nodeOld node
			// This is very dependent on the random mechanism in golang map.
			for _, v1 := range offspring.nodeMap {
				nodeOld = v1
				for k2, v2 := range v1.genomeMap {
					genOld = k2
					omeOld = v2
					break
				}
				break
			}
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
			for i := 0; i < length; {
				if offspring.nodeSlice[i] == genOld.out {
					newSlice[count] = n.nodeNum
					count++
				}
				newSlice[count] = offspring.nodeSlice[i]
				count++
				i++
			}
		} else if ran >= p1 && ran < p1+p2 {
			var tempSlice []int
			for _, v := range offspring.nodeSlice {
				if offspring.nodeMap[v].nodeType == 1 {
					tempSlice = append(tempSlice, v)
				}
			}
			if len(tempSlice) != 0 {
				var index = tempSlice[r.Intn(len(tempSlice))]
				var pointer int
				for k, v := range offspring.nodeSlice {
					if v == index {
						pointer = k
						break
					}
				}
				for i := pointer + 1; i < len(offspring.nodeSlice); i++ {
					var outputNum = offspring.nodeSlice[i]
					var n = offspring.nodeMap[outputNum]
					delete(n.genomeMap, gen{index, outputNum})
					offspring.nodeMap[outputNum] = n
				}
				delete(offspring.nodeMap, index)
			}
		} else if ran >= p1+p2 && ran < p1+p2+p3 {
			var length = len(offspring.nodeSlice)
			var index1 = r.Intn(length)
			var index2 = index1 + r.Intn(length-index1)
			if index1 != index2 {
				var inputNum = offspring.nodeSlice[index1]
				var outputNum = offspring.nodeSlice[index2]
				var inputNode = offspring.nodeMap[inputNum]
				var outputNode = offspring.nodeMap[outputNum]
				if !((inputNode.nodeType == 0 && outputNode.nodeType == 0) || (inputNode.nodeType == 2 && outputNode.nodeType == 2)) {
					var g = gen{inputNum, outputNum}
					var _, ok = outputNode.genomeMap[g]
					if !ok {
						var o = ome{random(), true, offspring.innovationNum}
						offspring.innovationNum++
						outputNode.genomeMap[g] = o
						offspring.nodeMap[outputNum] = outputNode
					}
				}
			}
		} else {
			var g gen
			var n node
			// This is very dependent on the random mechanism in golang map.
			for _, v1 := range offspring.nodeMap {
				n = v1
				for k2 := range v1.genomeMap {
					g = k2
					break
				}
				break
			}
			delete(n.genomeMap, g)
			offspring.nodeMap[g.out] = n
		}
	}
	return offspring
}
