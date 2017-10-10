package main

import (
	"fmt"
	"runtime"
	"./LycorisNet"
)

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	fmt.Println("Lycoris core 0.9.4")
	var test = LycorisNet.NewIndividual(10, 10)
	fmt.Println(len(test.GenomeList),len(test.NodeList))
}
