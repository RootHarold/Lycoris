/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
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
     * The class Args stores the super parameters needed in
     * various calculation processes of neural networks.
     */

    class Args {
    public:
        Args();

        ~Args();

        friend class LycorisUtils;

        friend class Lycoris;

        friend class Individual;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

        friend Lycoris *loadViaString(const std::string &model, uint32_t capacity);

    private:
        // An object of the class LycorisUtils which is integrated into this.
        LycorisUtils *utils;

        // Add the new node between a connection.
        float p1;
        // Delete a node.
        float p2;
        // Add a new connection between two nodes.
        float p3;
        // Delete a connection.
        float p4;
        // The backup of p1.
        float p1B;
        // The backup of p2.
        float p2B;
        // The backup of p3.
        float p3B;
        // The backup of p4.
        float p4B;

        // The number of logical cpu.
        uint32_t cpuNum;
        // The initial odds of mutating.
        float mutateOdds;
        // The lower bound of weight in initialization.
        float weightA;
        // The upper bound of weight in initialization.
        float weightB;

        // The learning rate.
        float lr;

        // The mode of Lycoris. (Classify or Predict)
        std::string mode;
        // Store the input array.
        std::vector<std::vector<float> > inputArray;
        // Store the desire array.
        std::vector<std::vector<float> > desireArray;
        // Store the batch size.
        uint32_t batchSize;
        // Store the batch size.
        uint32_t batchSize_;
        // Store the size of the best individual.
        uint32_t individualSize_;

        // When memory exceeds the limit, this changes to true.
        bool memOverFlag;
        // The flag of memory limit.
        bool memLimitFlag;
        // The size of maximum memory.
        uint32_t limitSize;
        // After the first execution, this changes to false.
        bool firstRun;
        // The array used to store data of batch size.
        float **batchData;
        // The flag of initializing batchData.
        bool batchFlag;
        // Used in BP_Multi_Thread().
        float *midData;
        // The depth of the neural network.
        uint32_t depth;
        // After the first execution of preheat(), this changes to false.
        bool preheatFlag;
        // After the first execution of evolve(), this changes to false.
        bool evolveFlag;
    };

}

#endif //LYCORIS_ARGS_H
