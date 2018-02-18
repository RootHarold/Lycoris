package main

import (
	"fmt"
	"./LycorisNet"
)

func f(in *LycorisNet.Individual) {

}

func main() {
	fmt.Println("Lycoris core 0.9.9-8")
	var ly = LycorisNet.NewLycoris(100, 10, 10)
	ly.SetForwardFunc(f)
	ly.RunLycoris()
}
