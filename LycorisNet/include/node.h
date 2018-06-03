/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_NODE_H
#define LYCORIS_NODE_H

#include <map>
#include "genome.h"

/*
    Every node contains its number, type and the value calculated
    during "forward" function. When it comes to the "nodeType", "0"
    means "input" while "1" means "hidden" and "2" means "output".
    Each node is the basic unit of the gene.
 */

class Node {
public:
    uint32_t nodeNum;
    uint32_t nodeType;
    float value;
    float bias;
    std::map<Gen, Ome> *genomeMap;

    Node(uint32_t nodeNum, uint32_t nodeType);

    ~Node();

    void initializeBias(float f);

    // Deep clone of node.
    Node *clone();
};

#endif //LYCORIS_NODE_H
