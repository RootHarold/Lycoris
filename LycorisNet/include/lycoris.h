/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include <vector>
#include <string>
#include "utils.h"

namespace LycorisNet {

    /*
     * The class Lycoris is the highest level abstraction of LycorisNet.
     */

    class Lycoris {
    public:

        /*
         * Constructor.
         *
         * capacity: capacity of Lycoris.
         *
         * inputDim: input dimension.
         *
         * outputDim: output dimension.
         *
         * mode: mode of Lycoris (classify or predict).
         */

        Lycoris(uint32_t capacity, uint32_t inputDim, uint32_t outputDim, const std::string &mode);

        ~Lycoris();

        void preheat(uint32_t num_of_nodes, uint32_t num_of_connections, uint32_t depth);

        void evolve(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire);

        void fitAll(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire);

        void fit(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire);

        void enrich();

        /*
         * Forward Computing of the best individual.
         *
         * input: Input data.
         *
         * Returns the output data.
         */

        std::vector<float> compute(std::vector<float> &input);

        /*
         * Resize the capacity of the neural network cluster.
         */

        void resize(uint32_t capacity);

        /*
         * Turn on memory-limit.
         */

        void openMemLimit(uint32_t size);

        /*
         * Turn off memory-limit.
         */

        void closeMemLimit();

        /*
         * Export the current trained model.
         *
         * path: File path of the current trained model.
         */

        void saveModel(const std::string &path);

        /*
         * Set p1 to p4 in the class Args.
         *
         * p1: Probability of adding the new node between a connection.
         *
         * p2: Probability of deleting a node.
         *
         * p3: Probability of adding a new connection between two nodes.
         *
         * p4: Probability of deleting a connection.
         */

        void setMutateArgs(std::vector<float> &p);

        /*
         * Set the odds of mutating.
         *
         * The param "odds" means one individual mutates odds times
         * to form odds + 1 individuals.
         */

        void setMutateOdds(float odds);

        /*
         * Set the number of worker threads to train the model.
         */

        void setCpuCores(uint32_t num);

        /*
         * Set the learning rate.
         */

        void setLR(float lr);

        /*
         * Returns the size of the best individual.
         */

        uint32_t getSize();

        /*
         * Returns the input dimension.
         */

        uint32_t getInputDim();

        /*
         * Returns the output dimension.
         */

        uint32_t getOutputDim();

        /*
         * Returns capacity of Lycoris.
         */

        uint32_t getCapacity();

        /*
         * Returns the loss.
         */

        float getFitness();

        /*
         * Returns mode of Lycoris (classify or predict).
         */

        std::string getMode();

        /*
         * Returns version information and copyright information.
         */

        inline static std::string version() {
            return "Lycoris Core 2.0-Developing (CPU Mode). Copyright (C) 2019 RootHarold. All rights reserved.";
        }

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

    private:
        // The dimension of input.
        uint32_t inputNum;
        // The dimension of output.
        uint32_t outputNum;
        // The capacity of Lycoris.
        uint32_t capacity;
        // The best individual.
        Individual *best;

        // An object of Args is integrated into this.
        Args *args;

        std::vector<Individual *> *individualList;
        std::vector<Individual *> tempList;
        // Store the length of individualList in speciesList.
        uint32_t oldLength;

        // Mutating.
        void mutate();

        // The parallel kernel of mutate().
        void mutateCore(uint32_t start, uint32_t end);

        // All individuals are calculated forward.
        void backPropagation();

        // The parallel kernel of forward().
        void backPropagationCore(uint32_t start, uint32_t end);

        // Choose elites and manipulate gapList.
        void chooseElite();

        // Each time this function is called, the network runs forward one time.
        void runLycoris();

        void preheatCore(uint32_t start, uint32_t end, uint32_t num_of_nodes, uint32_t num_of_connections);

        void checkFirstRun();
    };

    /*
     * Import the pre-trained model.
     *
     * path: File path of the pre-trained model.
     *
     * capacity: Capacity of the neural network cluster.
     *
     * Returns a pointer to the object of class Lycoris.
     */

    Lycoris *loadModel(const std::string &path, uint32_t capacity);

}

#endif //LYCORIS_LYCORIS_H
