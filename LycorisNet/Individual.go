package LycorisNet

import (
	"log"
	"os"
	"bytes"
	"compress/gzip"
	"strconv"
	"io/ioutil"
	"strings"
	"unsafe"
)

// The "innovationNum" is the cumulative number of connections
// and the "nodeSum" is that of nodes. High fitness means the
// individual is well adapted to the environment. The "nodeMap"
// and the "nodeSlice" store nodes.
type Individual struct {
	nodeMap       map[int]node
	nodeSlice     []int
	InputNum      int
	OutputNum     int
	innovationNum int
	nodeSum       int
	Fitness       float32
}

// Create a new individual.
func newIndividual(inputNum int, outputNum int) *Individual {
	var in = &Individual{InputNum: inputNum, OutputNum: outputNum, nodeSum: 0, innovationNum: 0}
	initialize(in)
	return in
}

// Initialize a individual.
func initialize(in *Individual) {
	var sumNode = in.InputNum + in.OutputNum
	in.nodeMap = make(map[int]node, sumNode)
	in.nodeSlice = make([]int, sumNode)
	for i := 0; i < in.InputNum; i++ {
		var temp = in.nodeSum
		in.nodeMap[temp] = *newNode(temp, 0)
		in.nodeSlice[temp] = temp
		in.nodeSum++
	}
	for i := 0; i < in.OutputNum; i++ {
		var temp = in.nodeSum
		in.nodeMap[temp] = *newNode(temp, 2)
		in.nodeSlice[temp] = temp
		in.nodeSum++
	}
}

// Set input array.
func (in *Individual) SetInput(input []float32) {
	for i := 0; i < in.InputNum; i++ {
		var temp = in.nodeMap[i]
		temp.value = input[i]
		in.nodeMap[i] = temp
	}
}

// Get output array.
func (in *Individual) GetOutput() []float32 {
	var output = make([]float32, in.OutputNum)
	var pointer = 0
	for i := in.InputNum; i < len(in.nodeSlice); i++ {
		var temp = in.nodeMap[in.nodeSlice[i]]
		if temp.nodeType == 2 {
			output[pointer] = temp.value
			pointer++
		}
	}
	return output
}

// The odds of cleaning in "Forward()".
var cleanOdds float32 = 0.008

// Forward calculation of the individual.
func (in *Individual) Forward() {
	var clean = make(map[int]bool)
	for i := in.InputNum; i < len(in.nodeSlice); i++ {
		var index = in.nodeSlice[i]
		var n = in.nodeMap[index]
		var f float32 = 0
		if len(n.genomeMap) == 0 {
			if n.nodeType == 1 {
				clean[index] = true
			}
		} else {
			for g, o := range n.genomeMap {
				if o.isEnable {
					f += in.nodeMap[g.in].value * o.weight
				}
			}
			n.value = activateFunc(f + n.bias)
			in.nodeMap[index] = n
		}
	}

	// Clean empty nodes according to a certain chance (cleanOdds).
	if LycorisRandomFloat32() < cleanOdds && len(clean) != 0 {
		for k := range clean {
			delete(in.nodeMap, k)
		}

		var tempSlice = make([]int, len(in.nodeSlice)-len(clean))
		var count = 0
		for _, v := range in.nodeSlice {
			var _, ok = clean[v]
			if !ok {
				tempSlice[count] = v
				count++
			}
		}
		in.nodeSlice = tempSlice
	}
}

// Deep clone of individual.
func (in *Individual) clone() *Individual {
	var duplicate = &Individual{InputNum: in.InputNum, OutputNum: in.OutputNum, innovationNum: in.innovationNum, nodeSum: in.nodeSum}
	duplicate.nodeMap = make(map[int]node, len(in.nodeMap))
	for k, v := range in.nodeMap {
		duplicate.nodeMap[k] = *v.clone()
	}
	duplicate.nodeSlice = make([]int, len(in.nodeSlice))
	copy(duplicate.nodeSlice, in.nodeSlice)
	return duplicate
}

// To compact the damn code.
func checkErr(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

// Export the individual using compress/gzip. The reason why using
// string to store data is that I just cant't use encoding/binary.
func (in *Individual) Export(path string) {
	// Serialize the data.
	var data bytes.Buffer
	data.WriteString(strconv.Itoa(in.InputNum))
	data.WriteString(" ")
	data.WriteString(strconv.Itoa(in.OutputNum))
	data.WriteString(" ")
	data.WriteString(strconv.Itoa(in.innovationNum))
	data.WriteString(" ")
	data.WriteString(strconv.Itoa(in.nodeSum))
	data.WriteString(" ")
	data.WriteString(strconv.FormatFloat(float64(in.Fitness), 'E', -1, 32))
	data.WriteString(" ")
	data.WriteString(strconv.Itoa(len(in.nodeSlice)))
	for _, v := range in.nodeSlice {
		data.WriteString(" ")
		data.WriteString(strconv.Itoa(v))
	}
	data.WriteString(" ")
	data.WriteString(strconv.Itoa(len(in.nodeMap)))
	for k1, v1 := range in.nodeMap {
		data.WriteString(" ")
		data.WriteString(strconv.Itoa(k1))
		data.WriteString(" ")
		data.WriteString(strconv.Itoa(v1.nodeNum))
		data.WriteString(" ")
		data.WriteString(strconv.Itoa(v1.nodeType))
		data.WriteString(" ")
		data.WriteString(strconv.FormatFloat(float64(v1.value), 'E', -1, 32))
		data.WriteString(" ")
		data.WriteString(strconv.FormatFloat(float64(v1.bias), 'E', -1, 32))
		data.WriteString(" ")
		data.WriteString(strconv.Itoa(len(v1.genomeMap)))
		for k2, v2 := range v1.genomeMap {
			data.WriteString(" ")
			data.WriteString(strconv.Itoa(k2.in))
			data.WriteString(" ")
			data.WriteString(strconv.Itoa(k2.out))
			data.WriteString(" ")
			data.WriteString(strconv.FormatFloat(float64(v2.weight), 'E', -1, 32))
			if v2.isEnable {
				data.WriteString(" 1 ")
			} else {
				data.WriteString(" 0 ")
			}
			data.WriteString(strconv.Itoa(v2.innovationNum))
		}
	}
	data.WriteString(" ")
	data.WriteString(activateFuncName)
	// Compress.
	var buf bytes.Buffer
	zw := gzip.NewWriter(&buf)
	_, err := zw.Write(data.Bytes())
	checkErr(err)
	if err := zw.Close(); err != nil {
		log.Fatal(err)
	}

	f, err := os.OpenFile(path, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0666)
	checkErr(err)
	_, err = buf.WriteTo(f)
	checkErr(err)
	f.Close()
}

// Import the individual.
func ImportIndividual(path string) *Individual {
	// Decompress.
	f, err := os.OpenFile(path, os.O_RDONLY, 0666)
	checkErr(err)
	var buf bytes.Buffer
	buf.ReadFrom(f)
	zr, err := gzip.NewReader(&buf)
	var dataBytes []byte
	dataBytes, err = ioutil.ReadAll(zr)
	checkErr(err)
	if err := zr.Close(); err != nil {
		log.Fatal(err)
	}
	f.Close()
	var data = strings.Split(string(dataBytes), " ")
	// Deserialize the data.
	var in = &Individual{}
	var temp int
	temp, err = strconv.Atoi(data[0])
	checkErr(err)
	in.InputNum = temp
	temp, err = strconv.Atoi(data[1])
	checkErr(err)
	in.OutputNum = temp
	temp, err = strconv.Atoi(data[2])
	checkErr(err)
	in.innovationNum = temp
	temp, err = strconv.Atoi(data[3])
	checkErr(err)
	in.nodeSum = temp
	var tempFloat float64
	tempFloat, err = strconv.ParseFloat(data[4], 32)
	checkErr(err)
	in.Fitness = float32(tempFloat)
	temp, err = strconv.Atoi(data[5])
	checkErr(err)
	in.nodeSlice = make([]int, temp)
	var pointer = 6
	for k := range in.nodeSlice {
		temp, err = strconv.Atoi(data[pointer])
		pointer++
		checkErr(err)
		in.nodeSlice[k] = temp
	}
	var mapLength int
	mapLength, err = strconv.Atoi(data[pointer])
	pointer++
	checkErr(err)
	in.nodeMap = make(map[int]node, mapLength)
	for i := 0; i < mapLength; i++ {
		var key int
		key, err = strconv.Atoi(data[pointer])
		pointer++
		checkErr(err)
		var n = node{}
		temp, err = strconv.Atoi(data[pointer])
		pointer++
		checkErr(err)
		n.nodeNum = temp
		temp, err = strconv.Atoi(data[pointer])
		pointer++
		checkErr(err)
		n.nodeType = temp
		tempFloat, err = strconv.ParseFloat(data[pointer], 32)
		pointer++
		checkErr(err)
		n.value = float32(tempFloat)
		tempFloat, err = strconv.ParseFloat(data[pointer], 32)
		pointer++
		checkErr(err)
		n.bias = float32(tempFloat)
		var genomeLength int
		genomeLength, err = strconv.Atoi(data[pointer])
		pointer++
		checkErr(err)
		n.genomeMap = make(map[gen]ome)
		for j := 0; j < genomeLength; j++ {
			var g = gen{}
			var o = ome{}
			temp, err = strconv.Atoi(data[pointer])
			pointer++
			checkErr(err)
			g.in = temp
			temp, err = strconv.Atoi(data[pointer])
			pointer++
			checkErr(err)
			g.out = temp
			tempFloat, err = strconv.ParseFloat(data[pointer], 32)
			pointer++
			checkErr(err)
			o.weight = float32(tempFloat)
			var boolValue = data[pointer]
			pointer++
			if boolValue == "1" {
				o.isEnable = true
			} else {
				o.isEnable = false
			}
			temp, err = strconv.Atoi(data[pointer])
			pointer++
			checkErr(err)
			o.innovationNum = temp
			n.genomeMap[g] = o
		}
		in.nodeMap[key] = n
	}
	SetActivateFunc(data[pointer])
	return in
}

// Return the size of the individual.
func (in *Individual) getSize() int {
	var size = 0
	size += int(unsafe.Sizeof(*in))
	size += int(unsafe.Sizeof(in.nodeSlice))
	size += int(unsafe.Sizeof(in.nodeMap))
	for _, v := range in.nodeMap {
		size += int(unsafe.Sizeof(v.genomeMap))
	}
	return size
}
