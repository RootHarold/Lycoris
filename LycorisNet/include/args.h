/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_ARGS_H
#define LYCORIS_ARGS_H

#include <string>
#include <vector>
#include <thread>
#include <climits>
#include "utils.h"

namespace LycorisNet {

    class LycorisUtils;

    /*
     * Store most of the parameters.
     */
    class Args {
    public:
        Args();

        ~Args();

        friend class LycorisUtils;

        friend class Lycoris;

        friend class Individual;

    private:
        // An object of LycorisUtils is integrated into this.
        LycorisUtils *utils;

        // Add the new node between a connection.
        float p1;
        // Delete a node.
        float p2;
        // Add a new connection between two nodes.
        float p3;
        // Delete a connection.
        float p4;
        // Mutate the connection between nodes.
        float p5;
        // Mutate the bias.
        float p6;
        // The backup of p1.
        float p1B;
        // The backup of p2.
        float p2B;
        // The backup of p3.
        float p3B;
        // The backup of p4.
        float p4B;
        // The backup of p5.
        float p5B;
        // The backup of p6.
        float p6B;
        // Name of active function.
        std::string activateFuncName;

        // The number of logical cpu.
        uint32_t cpuNum;
        // The initial odds of mutating.
        float mutateOdds;
        // For SetWeightRandom(...).
        float weightA;
        float weightB;
        // For SetBiasRandom(...).
        float biasA;
        float biasB;

        // The odds of cleaning in "Forward()".
        float cleanOdds;

        // Active function.
        float (*activateFunc)(float);

        // When memory exceeds the limit, this changes to true.
        bool memOverFlag;
        // The flag of memory limit.
        bool memLimitFlag;
        // To fixed a potential bug.
        bool firstOver;
        // The size of maximum memory.
        uint32_t limitSize;
        // To fixed a potential bug.
        bool firstRun;
    };

}

#endif //LYCORIS_ARGS_H
