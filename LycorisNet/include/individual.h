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
#include "node.h"
#include "args.h"

/*
    The "innovationNum" is the cumulative number of connections
    and the "nodeSum" is that of nodes. High fitness means the
    individual is well adapted to the environment. The "nodeMap"
    and the "nodeSlice" store nodes.
 */

class Individual {
public:
    uint32_t inputNum;
    uint32_t outputNum;
    uint32_t innovationNum;
    uint32_t nodeSum;
    float fitness;
    std::vector<uint32_t> *nodeSlice;
    std::map<uint32_t, Node *> *nodeMap;
    Args *args;

    Individual();

    Individual(uint32_t inputNum, uint32_t outputNum, Args *args);

    ~Individual();

    // Initialize a individual.
    void initialize();

    // Set input array.
    void setInput(float *input);

    // Get output array.
    float *getOutput();

    // Forward calculation of the individual.
    void forward();

    // Deep clone of individual.
    Individual *clone();

    // Return the size of the individual.
    uint32_t getSize();
};

#endif //LYCORIS_INDIVIDUAL_H
