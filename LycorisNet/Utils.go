package LycorisNet

import (
	"time"
	"math/rand"
	"math"
)

var leak = 0.01
var r = rand.New(rand.NewSource(time.Now().UnixNano()))
var c1 = 1.0
var c2 = 0.4
var breakTime = 12
var p1 = 0.15
var p2 = 0.35
var p3 = 0.15
var mutateTime = 1

func random() float64 {
	return r.Float64()
}

func activateFunc(f float64) float64 {
	return reLU(f)
}

func reLU(f float64) float64 {
	if f > 0 {
		return f
	} else {
		return f * leak
	}
}

func sigmoid(f float64) float64 {
	return 1 / (1 + math.Exp(0-f))
}

func tanh(f float64) float64 {
	return (math.Exp(f) - math.Exp(-f)) / (math.Exp(f) + math.Exp(-f))
}

func sort1(m *map[gen]ome, innovationNum int) (*[]float64, *[]int) {
	var length = len(*m)
	var result1 = make([]float64, length)
	var result2 = make([]int, length)
	var temp1 = make([]bool, innovationNum)
	var temp2 = make([]float64, innovationNum)
	var temp3 = make([]int, innovationNum)
	var count = 0
	for _, v := range *m {
		in := v.innovationNum
		temp1[in] = true
		temp2[in] = v.weight
		temp3[in] = in
	}
	for k, v := range temp1 {
		if v {
			result1[count] = temp2[k]
			result2[count] = temp3[k]
			count++
		}
	}
	return &result1, &result2
}

func sort2(m *map[gen]ome, innovationNum int) (*[]gen, *[]ome) {
	var length = len(*m)
	var result1 = make([]gen, length)
	var result2 = make([]ome, length)
	var temp1 = make([]bool, innovationNum)
	var temp2 = make([]gen, innovationNum)
	var temp3 = make([]ome, innovationNum)
	var count = 0
	for k, v := range *m {
		in := v.innovationNum
		temp1[in] = true
		temp2[in] = k
		temp3[in] = v
	}
	for k, v := range temp1 {
		if v {
			result1[count] = temp2[k]
			result2[count] = temp3[k]
			count++
		}
	}
	return &result1, &result2
}

func error(output []float64, desire []float64) float64 {
	var f float64 = 0
	for i := 0; i < len(output); i++ {
		f += math.Pow(desire[i]-output[i], 2)
	}
	return f
}

func distance(in1 *individual, in2 *individual) float64 {
	var d float64
	var DE = 0
	var w1, i1 = sort1(&in1.genomeMap, in1.innovationNum)
	var w2, i2 = sort1(&in2.genomeMap, in2.innovationNum)
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

func mateIndividual(in1 *individual, in2 *individual) *individual {
	var offspring = individual{inputNum: in1.inputNum, outputNum: in1.outputNum}
	offspring.nodeMap = make(map[int]node)
	offspring.genomeMap = make(map[gen]ome)
	if in1.fitness > in2.fitness {
		var temp = in1
		in1 = in2
		in2 = temp
	}

	var g1, o1 = sort2(&in1.genomeMap, in1.innovationNum)
	var g2, o2 = sort2(&in2.genomeMap, in2.innovationNum)
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
				offspring.genomeMap[(*g1)[point1]] = (*o1)[point1]
			} else {
				offspring.genomeMap[(*g2)[point2]] = (*o2)[point2]
			}
			point1++
			point2++
		} else if (*o1)[point1].innovationNum > (*o2)[point2].innovationNum {
			point2++
		} else {
			offspring.genomeMap[(*g1)[point1]] = (*o1)[point1]
			point1++
		}
	}
	for i := point1; i < len1; i++ {
		offspring.genomeMap[(*g1)[point1]] = (*o1)[point1]
	}
	for i := point2; i < len2; i++ {
		offspring.genomeMap[(*g2)[point2]] = (*o2)[point2]
	}

	var temp1 = (*o1)[point1-1].innovationNum
	var temp2 = (*o2)[point2-1].innovationNum
	if temp1 > temp2 {
		offspring.innovationNum = temp1 + 1
	} else {
		offspring.innovationNum = temp2 + 1
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
		for k := range offspring.genomeMap {
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

	for k, v := range offspring.genomeMap {
		offspring.nodeMap[k.out].genomeMap[k] = v
	}

	if in1.nodeSum > in2.nodeSum {
		offspring.nodeSum = in1.nodeSum
	} else {
		offspring.nodeSum = in2.nodeSum
	}
	return &offspring
}

func mutateIndividual(in *individual) *individual {
	var offspring = in.clone()
	for z := 0; z < mutateTime; z++ {
		var ran = r.Float64()
		if ran < p1 {
			var genOld gen
			var omeOld ome
			for k, v := range offspring.genomeMap {
				genOld = k
				omeOld = v
				break
			}
			omeOld.isEnable = false
			offspring.genomeMap[genOld] = omeOld
			var n = *newNode(offspring.nodeSum, 1)
			offspring.nodeSum++
			var g1, o1 = newGenome(genOld.in, n.nodeNum, 1.0, true, offspring.innovationNum)
			offspring.innovationNum++
			var g2, o2 = newGenome(n.nodeNum, genOld.out, omeOld.weight, true, offspring.innovationNum)
			offspring.innovationNum++
			n.genomeMap[*g1] = *o1
			var nodeOld = offspring.nodeMap[genOld.out]
			var temp = nodeOld.genomeMap[genOld]
			temp.isEnable = false
			nodeOld.genomeMap[genOld] = temp
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
			offspring.genomeMap[*g1] = *o1
			offspring.genomeMap[*g2] = *o2
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
				for i := 0; i < pointer; i++ {
					delete(offspring.genomeMap, gen{offspring.nodeSlice[i], index})
				}
				for i := pointer + 1; i < len(offspring.nodeSlice); i++ {
					var outputNum = offspring.nodeSlice[i]
					var tempGen = gen{index, outputNum}
					var _, ok = offspring.genomeMap[tempGen]
					if ok {
						var n = offspring.nodeMap[outputNum]
						delete(n.genomeMap, tempGen)
						offspring.nodeMap[outputNum] = n
						delete(offspring.genomeMap, tempGen)
					}
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
					var _, ok = offspring.genomeMap[g]
					if !ok {
						var o = ome{random(), true, offspring.innovationNum}
						offspring.innovationNum++
						offspring.genomeMap[g] = o
						outputNode.genomeMap[g] = o
						offspring.nodeMap[outputNum] = outputNode
					}
				}
			}
		} else {
			var g gen
			for k := range offspring.genomeMap {
				g = k
				break
			}
			var n = offspring.nodeMap[g.out]
			delete(n.genomeMap, g)
			offspring.nodeMap[g.out] = n
			delete(offspring.genomeMap, g)
		}
	}
	return offspring
}
