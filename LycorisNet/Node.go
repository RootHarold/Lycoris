package LycorisNet

type node struct {
	nodeNum   int
	nodeType  int //0 means input while 1 means hidden and 2 means output
	value     float64
	genomeMap map[gen]ome
}

func newNode(nodeNum int, nodeType int) *node {
	var n = &node{nodeNum: nodeNum, nodeType: nodeType}
	n.genomeMap = make(map[gen]ome)
	return n
}

func (n *node) clone() *node {
	var duplicate = newNode(n.nodeNum, n.nodeType)
	duplicate.genomeMap = make(map[gen]ome, len(n.genomeMap))
	for k, v := range n.genomeMap {
		duplicate.genomeMap[k] = v
	}
	return duplicate
}
