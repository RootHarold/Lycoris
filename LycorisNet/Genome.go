package LycorisNet

type gen struct {
	in  int
	out int
}

type ome struct {
	weight        float64
	isEnable      bool
	innovationNum int
}

func newGenome(in int, out int, weight float64, isEnable bool, innovationNum int) (*gen, *ome) {
	var g = &gen{in, out}
	var o = &ome{weight, isEnable, innovationNum}
	return g, o
}
