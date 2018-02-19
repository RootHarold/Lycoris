package LycorisNet

// Used when sorting individuals.
type sort_fitness struct {
	fitness       float32
	specieNum     int
	individualNum int
}

type sort_fitness_array []sort_fitness

func (f sort_fitness_array) Len() int {
	return len(f)
}

func (f sort_fitness_array) Less(i, j int) bool {
	return f[i].fitness < f[j].fitness
}

func (f sort_fitness_array) Swap(i, j int) {
	f[i], f[j] = f[j], f[i]
}
