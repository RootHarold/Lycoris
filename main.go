package main

import (
	"fmt"
	"./LycorisNet"
)

func fun(in *LycorisNet.Individual) {
	in.SetInput([]float32{0, 1})
	in.Forward()
	var ret float32 = 0
	for i := 0; i < in.OutputNum; i++ {
		ret -= (in.GetOutput()[i] - 0.5) * (in.GetOutput()[i] - 0.5)
	}
	in.Fitness = ret * 10000
}

func main() {
	fmt.Println("Lycoris core 0.9.9-14")
	var radiata = LycorisNet.NewLycoris(1000, 2, 2)
	radiata.SetForwardFunc(fun)
	epoch := 10
	for i := 0; i < epoch; i++ {
		radiata.RunLycoris()
		fmt.Println(radiata.Best.Fitness)
	}
	radiata.Best.SetInput([]float32{0, 1})
	radiata.Best.Forward()
	fmt.Println(radiata.Best.GetOutput())
}
