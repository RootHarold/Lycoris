/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
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
        uint32_t inputNum;
        uint32_t outputNum;
        float fitness;

        Individual();

        Individual(uint32_t inputNum, uint32_t outputNum, Args *args);

        ~Individual();

        // Set input array => Forward calculation of the individual => Get output array.
        void forward(float *input, float *output);

        // Back Propagation.
        void BP();

        void BP_P();

        void BP_P_Core(uint32_t start, uint32_t end);

        void BP_Compute(uint32_t start, uint32_t end, float *midData);

        //void BP_built_in_core();

        friend class LycorisUtils;

        friend class Lycoris;

    private:
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
    };

}

#endif //LYCORIS_INDIVIDUAL_H
