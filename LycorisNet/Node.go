package LycorisNet

type Node struct {
	NodeNum   int
	NodeType  int //0 means input while 1 means hidden and 2 means output
	Value     float64
	GenomeMap map[Gen]Ome
}

func NewNode(NodeNum int, NodeType int) *Node {
	var node *Node = &Node{NodeNum: NodeNum, NodeType: NodeType}
	node.GenomeMap = make(map[Gen]Ome)
	return node
}

func (node *Node) Clone() *Node {
	var n = NewNode(node.NodeNum, node.NodeType)
	for k, v := range node.GenomeMap {
		n.GenomeMap[k] = v
	}
	return n
}
