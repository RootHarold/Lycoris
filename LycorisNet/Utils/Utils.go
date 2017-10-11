package Utils

import (
	"time"
	"math/rand"
)

func Random() float64 {
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	return r.Float64()
}
