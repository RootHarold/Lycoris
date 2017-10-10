package LycorisNet

type Node struct {
	NodeNum    int
	NodeType   int //0 means input while 1 means hidden and 2 means output
	Value      float64
	GenomeList []Genome
}

func NewNode(NodeNum int, NodeType int) *Node {
	var node *Node = &Node{NodeNum: NodeNum, NodeType: NodeType}
	return node
}

func (node *Node) Clone() *Node {
	var offspring = NewNode(node.NodeNum, node.NodeType)
	for _, v := range node.GenomeList {
		offspring.GenomeList = append(offspring.GenomeList, *v.Clone())
	}
	return offspring
}
