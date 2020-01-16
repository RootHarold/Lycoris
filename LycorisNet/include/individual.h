/*
    Copyright (c) 2020, RootHarold
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

    class Args;

    /*
     * Each object of the class Individual represents a neural network.
     */

    class Individual {
    public:
        Individual();

        Individual(uint32_t inputNum, uint32_t outputNum, Args *args);

        ~Individual();

        friend class LycorisUtils;

        friend class Lycoris;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

        friend Lycoris *loadViaString(const std::string &model, uint32_t capacity);

    private:
        // The input dimension of the neural network.
        uint32_t inputNum;
        // The output dimension of the neural network.
        uint32_t outputNum;
        // The result of cross_entropy() or euclidean_distance().
        float fitness;
        // The cumulative number of connections.
        uint32_t innovationNum;
        // The cumulative number of nodes.
        uint32_t nodeSum;
        // Store nodes.
        std::vector<uint32_t> *nodeSlice;
        // Store nodes.
        std::map<uint32_t, Node *> *nodeMap;
        // All the objects of the class Individual share an object of the class Args.
        Args *args;
        // Stores the number of nodes in each layer of the neural network.
        std::vector<uint32_t> layers;

        // Initialize a individual.
        void initialize();

        // Deep clone of individual.
        Individual *clone();

        // Return the size of the individual.
        uint32_t getSize();

        // Set input array => Forward calculation of the individual => Get output array.
        void forward(std::vector<float> &input, float *output);

        // Single threaded version of back propagation.
        void BP_Single_Thread();

        // Multi threaded version of back propagation.
        void BP_Multi_Thread();

        // One of the functor of BP_Multi_Thread().
        void BP_Multi_Thread_Core(uint32_t start, uint32_t end);

        // One of the functor of BP_Multi_Thread().
        void BP_Multi_Thread_Forward(uint32_t start, uint32_t end, float *midData);

        /*
         * Get a vector of nodes in a specific layer of this neural network.
         * The parameter pos starts at index 0.
         */
        std::vector<float> getHiddenLayer(uint32_t pos);
    };

}

#endif //LYCORIS_INDIVIDUAL_H
