package LycorisNet

type Genome struct {
	In            int
	Out           int
	Weight        float64
	IsEnable      bool
	InnovationNum int
}

func NewGenome(In int, Out int, Weight float64, IsEnable bool, InnovationNum int) *Genome {
	var genome *Genome = &Genome{In, Out, Weight, IsEnable, InnovationNum}
	return genome
}

func (genome *Genome) Clone() *Genome {
	var offspring *Genome = NewGenome(genome.In, genome.Out, genome.Weight, genome.IsEnable, genome.InnovationNum)
	return offspring
}
