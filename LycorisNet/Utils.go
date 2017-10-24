package LycorisNet

import (
	"time"
	"math/rand"
	"math"
)

var Leak = 0.01
var r = rand.New(rand.NewSource(time.Now().UnixNano()))
var C1 = 1.0
var C2 = 0.4
var BreakTime = 12
var P1 = 0.15
var P2 = 0.35
var P3 = 0.15
var MutateTime = 1

func Random() float64 {
	return r.Float64()
}

func Func(f float64) float64 {
	return ReLU(f)
}

func ReLU(f float64) float64 {
	if f > 0 {
		return f
	} else {
		return f * Leak
	}
}

func Sigmoid(f float64) float64 {
	return 1 / (1 + math.Exp(0-f))
}

func Tanh(f float64) float64 {
	return (math.Exp(f) - math.Exp(-f)) / (math.Exp(f) + math.Exp(-f))
}

func Sort1(m *map[Gen]Ome, InnovationNum int) (*[]float64, *[]int) {
	var length = len(*m)
	var result1 = make([]float64, length)
	var result2 = make([]int, length)
	var temp1 = make([]bool, InnovationNum)
	var temp2 = make([]float64, InnovationNum)
	var temp3 = make([]int, InnovationNum)
	var count = 0
	for _, v := range *m {
		in := v.InnovationNum
		temp1[in] = true
		temp2[in] = v.Weight
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

func Sort2(m *map[Gen]Ome, InnovationNum int) (*[]Gen, *[]Ome) {
	var length = len(*m)
	var result1 = make([]Gen, length)
	var result2 = make([]Ome, length)
	var temp1 = make([]bool, InnovationNum)
	var temp2 = make([]Gen, InnovationNum)
	var temp3 = make([]Ome, InnovationNum)
	var count = 0
	for k, v := range *m {
		in := v.InnovationNum
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

func Error(output []float64, desire []float64) float64 {
	var f float64 = 0
	for i := 0; i < len(output); i++ {
		f += math.Pow(desire[i]-output[i], 2)
	}
	return f
}

func Distance(in1 *Individual, in2 *Individual) float64 {
	var distance float64
	var DE = 0
	var w1, i1 = Sort1(&in1.GenomeMap, in1.InnovationNum)
	var w2, i2 = Sort1(&in2.GenomeMap, in2.InnovationNum)
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
	distance = (C1*float64(DE))/float64(N) + (C2*float64(W))/float64(countW)
	return distance
}

func MateIndividual(in1 *Individual, in2 *Individual) *Individual {
	var offspring = Individual{InputNum: in1.InputNum, OutputNum: in1.OutputNum}
	offspring.NodeMap = make(map[int]Node)
	offspring.GenomeMap = make(map[Gen]Ome)
	if in1.Fitness > in2.Fitness {
		var temp = in1
		in1 = in2
		in2 = temp
	}

	var g1, o1 = Sort2(&in1.GenomeMap, in1.InnovationNum)
	var g2, o2 = Sort2(&in2.GenomeMap, in2.InnovationNum)
	var point1 = 0
	var point2 = 0
	var len1 = len(*g1)
	var len2 = len(*g2)
	for true {
		if point1 == len1 || point2 == len2 {
			break
		}
		if (*o1)[point1].InnovationNum == (*o2)[point2].InnovationNum {
			if r.Float64() < 0.5 {
				offspring.GenomeMap[(*g1)[point1]] = (*o1)[point1]
			} else {
				offspring.GenomeMap[(*g2)[point2]] = (*o2)[point2]
			}
			point1++
			point2++
		} else if (*o1)[point1].InnovationNum > (*o2)[point2].InnovationNum {
			point2++
		} else {
			offspring.GenomeMap[(*g1)[point1]] = (*o1)[point1]
			point1++
		}
	}
	for i := point1; i < len1; i++ {
		offspring.GenomeMap[(*g1)[point1]] = (*o1)[point1]
	}
	for i := point2; i < len2; i++ {
		offspring.GenomeMap[(*g2)[point2]] = (*o2)[point2]
	}

	var temp1 = (*o1)[point1-1].InnovationNum
	var temp2 = (*o2)[point2-1].InnovationNum
	if temp1 > temp2 {
		offspring.InnovationNum = temp1 + 1
	} else {
		offspring.InnovationNum = temp2 + 1
	}

	var tempMap = make(map[int]int)
	for _, v := range in1.NodeMap {
		tempMap[v.NodeNum] = -1
	}
	for _, v := range in2.NodeMap {
		tempMap[v.NodeNum] = -1
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
		for k := range offspring.GenomeMap {
			var indexIn = tempMap[k.In]
			var indexOut = tempMap[k.Out]
			if indexIn > indexOut {
				flag = false
				tempSlice[indexIn] = k.Out
				tempSlice[indexOut] = k.In
				tempMap[k.Out] = indexIn
				tempMap[k.In] = indexOut
			}
		}
		if flag || bt == BreakTime {
			break
		}
	}

	var lenTempSlice = len(tempSlice)
	offspring.NodeSlice = make([]int, lenTempSlice)
	for i := 0; i < in1.InputNum; i++ {
		offspring.NodeSlice[i] = i
	}
	var temp = in1.InputNum + in1.OutputNum
	for i := in1.InputNum; i < temp; i++ {
		offspring.NodeSlice[lenTempSlice-temp+i] = i
	}
	var point = temp
	for _, v := range tempSlice {
		if v >= temp {
			offspring.NodeSlice[point] = v
			point++
		}
	}

	for _, v := range offspring.NodeSlice {
		var node Node
		if v < offspring.InputNum {
			node = *NewNode(v, 0)
		} else if v >= offspring.InputNum && v < temp {
			node = *NewNode(v, 2)
		} else {
			node = *NewNode(v, 1)
		}
		offspring.NodeMap[v] = node
	}

	for k, v := range offspring.GenomeMap {
		offspring.NodeMap[k.Out].GenomeMap[k] = v
	}

	if in1.NodeSum > in2.NodeSum {
		offspring.NodeSum = in1.NodeSum
	} else {
		offspring.NodeSum = in2.NodeSum
	}
	return &offspring
}

func MutateIndividual(in *Individual) *Individual {
	var offspring = in.Clone()
	for z := 0; z < MutateTime; z++ {
		var random = r.Float64()
		if random < P1 {
			var genOld Gen
			var omeOld Ome
			for k, v := range offspring.GenomeMap {
				genOld = k
				omeOld = v
				break
			}
			omeOld.IsEnable = false
			offspring.GenomeMap[genOld] = omeOld
			var n = *NewNode(offspring.NodeSum, 1)
			offspring.NodeSum++
			var g1, o1 = NewGenome(genOld.In, n.NodeNum, 1.0, true, offspring.InnovationNum)
			offspring.InnovationNum++
			var g2, o2 = NewGenome(n.NodeNum, genOld.Out, omeOld.Weight, true, offspring.InnovationNum)
			offspring.InnovationNum++
			n.GenomeMap[*g1] = *o1
			var nodeOld = offspring.NodeMap[genOld.Out]
			var temp = nodeOld.GenomeMap[genOld]
			temp.IsEnable = false
			nodeOld.GenomeMap[genOld] = temp
			nodeOld.GenomeMap[*g2] = *o2
			offspring.NodeMap[genOld.Out] = nodeOld
			offspring.NodeMap[n.NodeNum] = n
			var length = len(offspring.NodeSlice)
			var newSlice = make([]int, length+1)
			var count = 0
			for i := 0; i < length; {
				if offspring.NodeSlice[i] == genOld.Out {
					newSlice[count] = n.NodeNum
					count++
				}
				newSlice[count] = offspring.NodeSlice[i]
				count++
				i++
			}
			offspring.GenomeMap[*g1] = *o1
			offspring.GenomeMap[*g2] = *o2
		} else if random >= P1 && random < P1+P2 {
			var tempSlice []int
			for _, v := range offspring.NodeSlice {
				if offspring.NodeMap[v].NodeType == 1 {
					tempSlice = append(tempSlice, v)
				}
			}
			if len(tempSlice) != 0 {
				var index = tempSlice[r.Intn(len(tempSlice))]
				var pointer int
				for k, v := range offspring.NodeSlice {
					if v == index {
						pointer = k
						break
					}
				}
				for i := 0; i < pointer; i++ {
					delete(offspring.GenomeMap, Gen{offspring.NodeSlice[i], index})
				}
				for i := pointer + 1; i < len(offspring.NodeSlice); i++ {
					var outputNum = offspring.NodeSlice[i]
					var tempGen = Gen{index, outputNum}
					var _, ok = offspring.GenomeMap[tempGen]
					if ok {
						var node = offspring.NodeMap[outputNum]
						delete(node.GenomeMap, tempGen)
						offspring.NodeMap[outputNum] = node
						delete(offspring.GenomeMap, tempGen)
					}
				}
				delete(offspring.NodeMap, index)
			}
		} else if random >= P1+P2 && random < P1+P2+P3 {
			var length = len(offspring.NodeSlice)
			var index1 = r.Intn(length)
			var index2 = index1 + r.Intn(length-index1)
			if index1 != index2 {
				var inputNum = offspring.NodeSlice[index1]
				var outputNum = offspring.NodeSlice[index2]
				var inputNode = offspring.NodeMap[inputNum]
				var outputNode = offspring.NodeMap[outputNum]
				if !((inputNode.NodeType == 0 && outputNode.NodeType == 0) || (inputNode.NodeType == 2 && outputNode.NodeType == 2)) {
					var gen = Gen{inputNum, outputNum}
					var _, ok = offspring.GenomeMap[gen]
					if !ok {
						var ome = Ome{Random(), true, offspring.InnovationNum}
						offspring.InnovationNum++
						offspring.GenomeMap[gen] = ome
						outputNode.GenomeMap[gen] = ome
						offspring.NodeMap[outputNum] = outputNode
					}
				}
			}
		} else {
			var gen Gen
			for k := range offspring.GenomeMap {
				gen = k
				break
			}
			var node = offspring.NodeMap[gen.Out]
			delete(node.GenomeMap, gen)
			offspring.NodeMap[gen.Out] = node
			delete(offspring.GenomeMap, gen)
		}
	}
	return offspring
}
