/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_INDIVIDUAL_H
#define LYCORIS_INDIVIDUAL_H

#include <vector>
#include <map>
#include <cfloat>
#include "node.h"
#include "args.h"

namespace LycorisNet {

    /*
     * The "innovationNum" is the cumulative number of connections
     * and the "nodeSum" is that of nodes. High fitness means the
     * individual is well adapted to the environment. The "nodeMap"
     * and the "nodeSlice" store nodes.
     */

    class Args;

    class Individual {
    public:
        Individual();

        Individual(uint32_t inputNum, uint32_t outputNum, Args *args);

        ~Individual();

        friend class LycorisUtils;

        friend class Lycoris;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

    private:
        uint32_t inputNum;
        uint32_t outputNum;
        float fitness;
        uint32_t innovationNum;
        uint32_t nodeSum;
        std::vector<uint32_t> *nodeSlice;
        std::map<uint32_t, Node *> *nodeMap;
        Args *args;

        // Initialize a individual.
        void initialize();

        // Deep clone of individual.
        Individual *clone();

        // Return the size of the individual.
        uint32_t getSize();

        // Set input array => Forward calculation of the individual => Get output array.
        void forward(std::vector<float> &input, float *output);

        // Back Propagation.
        void BP_Single_Thread();

        void BP_Multi_Thread();

        void BP_Multi_Thread_Core(uint32_t start, uint32_t end);

        void BP_Multi_Thread_Forward(uint32_t start, uint32_t end, float *midData);
    };

}

#endif //LYCORIS_INDIVIDUAL_H
