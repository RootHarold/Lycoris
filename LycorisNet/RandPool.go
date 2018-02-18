package LycorisNet

import (
	"time"
	"math/rand"
)

// This solve the damn problem that random function is
// unsafe for concurrent use by multiple goroutines.

var chForFloat32 = make(chan float32, cpuNum*cpuNum)

// Daemon.
func randFloat32() {
	var r = rand.New(rand.NewSource(time.Now().UnixNano()))
	defer close(chForFloat32)
	for true {
		data := r.Float32()
		chForFloat32 <- data
	}
}

// Generate a random number of 0-1 (float32).
func GetRandomFloat32() float32 {
	return <-chForFloat32
}

// Generate a random number of 0-N (int).
func GetRandomInt(N int) int {
	return int(GetRandomFloat32() * float32(N))
}
