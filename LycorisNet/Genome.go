package LN

type Gen struct {
	In  int
	Out int
}

type Ome struct {
	Weight        float64
	IsEnable      bool
	InnovationNum int
}

func NewGenome(In int, Out int, Weight float64, IsEnable bool, InnovationNum int) (*Gen, *Ome) {
	var gen = &Gen{In, Out}
	var ome = &Ome{Weight, IsEnable, InnovationNum}
	return gen, ome
}
