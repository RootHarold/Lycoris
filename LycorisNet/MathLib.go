package LycorisNet

import (
	"math"
)

// Just like "math.Exp(...)".
func LycorisExp(x float32) float32 {
	return float32(math.Exp(float64(x)))
}

// Just like "math.Pow(...)".
func LycorisPow(x float32, y float32) float32 {
	return float32(math.Pow(float64(x), float64(y)))
}

// Just like "math.Abs(...)".
func LycorisAbs(x float32) float32 {
	return float32(math.Abs(float64(x)))
}

// It's for the function "ReLU(...)".
var leak float32 = 0.01

// ReLU.
func ReLU(f float32) float32 {
	if f > 0 {
		return f
	} else {
		return f * leak
	}
}

// Sigmoid.
func Sigmoid(f float32) float32 {
	return 1 / (1 + LycorisExp(0-f))
}

// Tanh.
func Tanh(f float32) float32 {
	return (LycorisExp(f) - LycorisExp(-f)) / (LycorisExp(f) + LycorisExp(-f))
}

// Softmax.
func Softmax(input []float32) []float32 {
	var length = len(input)
	var ret = make([]float32, length)
	var tempSlice = make([]float32, length)
	var sum float32 = 0
	for k, v := range input {
		var temp = LycorisPow(v, 2)
		tempSlice[k] = temp
		sum += temp
	}
	for k := range input {
		ret[k] = tempSlice[k] / sum
	}
	return ret
}