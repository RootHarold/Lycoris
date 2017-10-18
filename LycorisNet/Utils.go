package LycorisNet

import (
	"time"
	"math/rand"
	"math"
)

var Leak = 0.01
var r = rand.New(rand.NewSource(time.Now().UnixNano()))
var c1 = 1.0
var c2 = 0.4

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
	distance = (c1*float64(DE))/float64(N) + (c2*float64(W))/float64(countW)
	return distance
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

func mateIndividual(in1 *Individual, in2 *Individual) *Individual {
	var offspring = Individual{InputNum: in1.InputNum, OutputNum: in1.OutputNum}
	offspring.NodeMap = make(map[int]Node)
	offspring.GenomeMap = make(map[Gen]Ome)
	if in1.Fitness > in2.Fitness {
		var temp = in1
		in1 = in2
		in2 = temp
	}
	// TODO
	return &offspring
}
