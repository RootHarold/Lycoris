package LycorisNet

// Every node contains its number, type and the value calculated
// during "forward" function. When it comes to the "nodeType", "0"
// means "input" while "1" means "hidden" and "2" means "output".
// Each node is the basic unit of the gene.
type node struct {
	nodeNum   int
	nodeType  int
	value     float64
	genomeMap map[gen]ome
}

// Create new node with its number and type.
func newNode(nodeNum int, nodeType int) *node {
	var n = &node{nodeNum: nodeNum, nodeType: nodeType}
	n.genomeMap = make(map[gen]ome)
	return n
}

// Deep clone of node.
func (n *node) clone() *node {
	var duplicate = newNode(n.nodeNum, n.nodeType)
	duplicate.genomeMap = make(map[gen]ome, len(n.genomeMap))
	for k, v := range n.genomeMap {
		duplicate.genomeMap[k] = v
	}
	return duplicate
}
