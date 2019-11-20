/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_NODE_H
#define LYCORIS_NODE_H

#include <map>
#include "genome.h"

namespace LycorisNet {

    /*
     * Every node contains its number, type and the value calculated
     * during "forward" function. When it comes to the "nodeType", "0"
     * means "input" while "1" means "hidden" and "2" means "output".
     * Each node is the basic unit of the gene.
     */

    class Node {
    public:
        Node(uint32_t nodeNum, uint32_t nodeType);

        ~Node();

        friend class Lycoris;

        friend class LycorisUtils;

        friend class Individual;

    private:
        uint32_t nodeNum;
        uint32_t nodeType;
        float value;
        float bias;
        float delta;
        float delta_backup;
        std::map<Gen, Ome> *genomeMap;

        void initializeBias(float f);

        // Deep clone of Node.
        Node *clone();
    };

}

#endif //LYCORIS_NODE_H
