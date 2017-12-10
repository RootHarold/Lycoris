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

// This is for initializing weight.
func weightRandom() float64 {
	return r.Float64()
}

// This is for initializing bias.
func biasRandom() float64 {
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

	return &offspring
}

// Mutating the individual.
func mutateIndividual(in *individual) *individual {
	var offspring = in.clone()

	return offspring
}
