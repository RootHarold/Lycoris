package LycorisNet

import (
	"math"
)

// Just like "math.Exp(...)".
func lycorisExp(x float32) float32 {
	return float32(math.Exp(float64(x)))
}

// Just like "math.Pow(...)".
func lycorisPow(x float32, y float32) float32 {
	return float32(math.Pow(float64(x), float64(y)))
}

// Just like "math.Abs(...)".
func lycorisAbs(x float32) float32 {
	return float32(math.Abs(float64(x)))
}

// ReLU.
func reLU(f float32) float32 {
	if f > 0 {
		return f
	} else {
		return f * leak
	}
}

// Sigmoid.
func sigmoid(f float32) float32 {
	return 1 / (1 + lycorisExp(0-f))
}

// Tanh.
func tanh(f float32) float32 {
	return (lycorisExp(f) - lycorisExp(-f)) / (lycorisExp(f) + lycorisExp(-f))
}