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

// Used in "distance(...)".
func sort1(in *Individual) (*[]float32, *[]int) {
	var temp1 = make([]bool, in.innovationNum)
	var temp2 = make([]float32, in.innovationNum)
	var temp3 = make([]int, in.innovationNum)

	var length = 0
	for _, v1 := range in.nodeMap {
		length += len(v1.genomeMap)
		for _, v2 := range v1.genomeMap {
			num := v2.innovationNum
			temp1[num] = true
			temp2[num] = v2.weight
			temp3[num] = num
		}
	}

	var result1 = make([]float32, length)
	var result2 = make([]int, length)
	var count = 0
	for k, v := range temp1 {
		if v {
			result1[count] = temp2[k]
			result2[count] = temp3[k]
			count++
		}
	}
	return &result1, &result2
}

// Used in "mateIndividual(...)".
func sort2(in *Individual) (*[]gen, *[]ome) {
	var temp1 = make([]bool, in.innovationNum)
	var temp2 = make([]gen, in.innovationNum)
	var temp3 = make([]ome, in.innovationNum)

	var length = 0
	for _, v1 := range in.nodeMap {
		length += len(v1.genomeMap)
		for k, v2 := range v1.genomeMap {
			num := v2.innovationNum
			temp1[num] = true
			temp2[num] = k
			temp3[num] = v2
		}
	}

	var result1 = make([]gen, length)
	var result2 = make([]ome, length)
	var count = 0
	for k, v := range temp1 {
		if v {
			result1[count] = temp2[k]
			result2[count] = temp3[k]
			count++
		}
	}
	return &result1, &result2
}
