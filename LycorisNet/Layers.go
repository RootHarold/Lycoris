package LycorisNet

// This file provides some useful layers.

func softmax(input []float32) []float32 {
	var length = len(input)
	var ret = make([]float32, length)
	var tempSlice = make([]float32, length)
	var sum float32 = 0
	for k, v := range input {
		var temp = lycorisPow(v, 2)
		tempSlice[k] = temp
		sum += temp
	}
	for k := range input {
		ret[k] = tempSlice[k] / sum
	}
	return ret
}
