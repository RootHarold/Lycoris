package LycorisNet

// Used when sorting individuals.
type sortFitness struct {
	fitness       float32
	specieNum     int
	individualNum int
}

type sortFitnessArray []sortFitness

func (f sortFitnessArray) Len() int {
	return len(f)
}

func (f sortFitnessArray) Less(i, j int) bool {
	return f[i].fitness < f[j].fitness
}

func (f sortFitnessArray) Swap(i, j int) {
	f[i], f[j] = f[j], f[i]
}
