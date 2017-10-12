package LycorisNet

import (
	"time"
	"math/rand"
)

var Leak float64 = 0.01
var r = rand.New(rand.NewSource(time.Now().UnixNano()))

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
