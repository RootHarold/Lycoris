/*
    Copyright (c) 2020, RootHarold
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
     * Each node contains its number, type, the value used in
     * forward calculation, bias, gradient and the backup of
     * gradient. When it comes to "nodeType", "0" means "input"
     * while "1" means "hidden" and "2" means "output". Each
     * node is the basic unit of the neural network.
     */

    class Node {
    public:
        Node(uint32_t nodeNum, uint32_t nodeType);

        ~Node();

        friend class Lycoris;

        friend class LycorisUtils;

        friend class Individual;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

        friend Lycoris *loadViaString(const std::string &model, uint32_t capacity);

    private:
        // The number of the node.
        uint32_t nodeNum;
        // The type of the node.
        uint32_t nodeType;
        // The value used in forward calculation.
        float value;
        // Just bias.
        float bias;
        // Gradient.
        float delta;
        // The backup of gradient.
        float delta_backup;

        /*
         * A std::map type object that stores all connections to
         * the current node. This object is used in the forward
         * calculation and back propagation of the neural network.
         */

        std::map<Gen, Ome> *genomeMap;

        // Deep clone of Node.
        Node *clone();
    };

}

#endif //LYCORIS_NODE_H
