package LycorisNet

// The struct "gen" and the struct "ome" form connections
// between nodes. The former contains the numbers of input
// and output. The latter consists of weight, usability and
// the cumulative number of connections ("innovationNum").

type gen struct {
	in  int
	out int
}

type ome struct {
	weight        float64
	isEnable      bool
	innovationNum int
}

// Create new connections with full arguments.
func newGenome(in int, out int, weight float64, isEnable bool, innovationNum int) (*gen, *ome) {
	var g = &gen{in, out}
	var o = &ome{weight, isEnable, innovationNum}
	return g, o
}
