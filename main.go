package main

import (
	"fmt"
	"runtime"
)

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	fmt.Println("Lycoris core 0.9.8-19")
}
