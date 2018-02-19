package main

import (
	"fmt"
	"./LycorisNet"
)

func fun(in *LycorisNet.Individual) {

}

func main() {
	fmt.Println("Lycoris core 0.9.9-10")
	var radiata = LycorisNet.NewLycoris(100, 10, 10)
	radiata.SetForwardFunc(fun)
	radiata.RunLycoris()
}
