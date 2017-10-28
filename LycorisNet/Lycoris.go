package LycorisNet

type Lycoris struct {
	input       []float64
	desire      []float64
	speciesList []species
}

func NewLycoris(capacity int, inputNum int, outputNum int) *Lycoris {
	var lycoris = &Lycoris{}
	lycoris.input = make([]float64, inputNum)
	lycoris.desire = make([]float64, outputNum)
	var specie = species{}
	specie.individualList = make([]individual, capacity)
	for i := 0; i < capacity; i++ {
		specie.individualList[i] = *newIndividual(inputNum, outputNum)
	}
	lycoris.speciesList = append(lycoris.speciesList, specie)
	return lycoris
}

func (lycoris *Lycoris) chooseElite() {
	// TODO
}

func (lycoris *Lycoris) mate() {
	// TODO
}

func (lycoris *Lycoris) mutate() {
	// TODO
}

func (lycoris *Lycoris) classify() {
	// TODO
}

func (lycoris *Lycoris) RunLycoris() {
	lycoris.chooseElite()
	lycoris.mate()
	lycoris.mutate()
}

// TODO
/*
func (lycoris *Lycoris) GetTheBest(num int) ([]Individual) {

}
*/

func (lycoris *Lycoris) SetInput(input []float64) {
	for k, v := range input {
		lycoris.input[k] = v
	}
}

func (lycoris *Lycoris) SetDesire(desire []float64) {
	for k, v := range desire {
		lycoris.desire[k] = v
	}
}
