/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#include "node.h"

namespace LycorisNet {

    Node::Node(uint32_t nodeNum, uint32_t nodeType) {
        this->nodeNum = nodeNum;
        this->nodeType = nodeType;
        value = 0;
        delta = 0;
        bias = 0;
        genomeMap = new std::map<Gen, Ome>();
    }

    Node::~Node() {
        delete genomeMap;
    }

    /*
     * The implement of deep cloning. The return value is a
     * pointer to the copied Node object.
     */

    Node *Node::clone() {
        auto duplicate = new Node(nodeNum, nodeType);
        duplicate->bias = bias;
        duplicate->delta = delta;

        for (auto & iter : *genomeMap) {
            duplicate->genomeMap->insert(std::make_pair(iter.first, iter.second));
        }

        return duplicate;
    }

}
