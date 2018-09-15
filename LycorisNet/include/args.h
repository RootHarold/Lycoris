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

        // Arguments in distance(...).
        float c1;
        float c2;

        // These change the mutation probability.

        // Add the new node between a connection.
        float p1;
        // Delete a node.
        float p2;
        // Add a new connection between two nodes.
        float p3;
        // Delete a connection.
        float p4;
        // Just create a new empty node (without any genomes).
        float p5;
        // Mutate the bias.
        float p6;
        // Name of active function.
        std::string activateFuncName;

        // The number of mutations.
        uint32_t mutateTime;
        // The number of logical cpu.
        uint32_t cpuNum;
        // The initial odds of mating.
        float mateOdds;
        // The initial odds of mutating.
        float mutateOdds;
        // The maximum number of mutating.
        uint32_t maxMutateTime;
        // The threshold of the distance between two individuals.
        float distanceThreshold;
        // For SetWeigthRandom(...).
        float weightA;
        float weightB;
        // For SetBiasRandom(...).
        float biasA;
        float biasB;

        // The odds of cleaning in "Forward()".
        float cleanOdds;

        // Active function.
        float (*activateFunc)(float);

        // The role is to automatically change some of the parameters.
        std::vector<float> *gapList;
        // The length of gapList.
        uint32_t maxGap;
        // When memory exceeds the limit, this changes to true.
        bool memOverFlag;
        // The flag of memory limit.
        bool memLimitFlag;
        // To fixed a potential bug.
        bool firstOver;
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
        // The backup of mateOdds.
        float mateOddsB;
        // The backup of mutateOdds.
        float mutateOddsB;
        // The backup of mutateTime.
        uint32_t mutateTimeB;
        // The size of maximum memory.
        uint32_t limitSize;
        // Assist in chooseElite().
        bool gapListFlag;
        // To fixed a potential bug.
        bool firstRun;

        /*
         * The following parameters are used in autoParameter().
         */
        uint32_t slopeTick;
        uint32_t maxSlopeTick;
        uint32_t emergeTick;
        uint32_t emergeTock;
        uint32_t maxEmergeTock;
        uint32_t miss;
        uint32_t hit;
        bool checkFlag;
    };

}

#endif //LYCORIS_ARGS_H
