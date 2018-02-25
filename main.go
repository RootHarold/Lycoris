package main

import (
	"fmt"
	"./LycorisNet"
	"time"
)

func fun(in *LycorisNet.Individual) {
	in.SetInput([]float32{0, 1})
	in.Forward()
	var ret float32 = 0
	for i := 0; i < in.OutputNum; i++ {
		ret -= (in.GetOutput()[i] - 0.5) * (in.GetOutput()[i] - 0.5)
	}
	in.Fitness = ret
}

func main() {
	fmt.Println("Lycoris core 0.9.9-26")
	var radiata = LycorisNet.NewLycoris(1000, 2, 2)
	radiata.SetForwardFunc(fun)
	epoch := 10
	t1 := time.Now()
	for i := 0; i < epoch; i++ {
		radiata.RunLycoris()
		fmt.Println(radiata.Best.Fitness)
	}
	elapsed := time.Since(t1)
	fmt.Println(radiata.Best.GetOutput())
	fmt.Println("App elapsed: ", elapsed)
}
