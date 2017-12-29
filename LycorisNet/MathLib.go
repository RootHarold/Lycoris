package LycorisNet

import (
	"math"
)

func lycorisExp(x float32) float32 {
	return float32(math.Exp(float64(x)))
}

func lycorisPow(x float32, y float32) float32 {
	return float32(math.Pow(float64(x), float64(y)))
}

func lycorisAbs(x float32) float32 {
	return float32(math.Abs(float64(x)))
}
