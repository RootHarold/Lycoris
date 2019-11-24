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
     * The class Lycoris is the highest level module of LycorisNet.
     */

    class Lycoris {
    public:
        Lycoris(uint32_t capacity, uint32_t inputNum, uint32_t outputNum, const std::string &mode);

        ~Lycoris();

        void preheat(uint32_t num_of_nodes, uint32_t num_of_connections);

        // Set the input array and the desire array. And then run forward.
        void evolve(float **input, float **desire, uint32_t batchSize);

        // Set the input array and the desire array. And run forward n times.
        void evolve(float **input, float **desire, uint32_t batchSize, uint32_t n);

        void fit(float **input, float **desire, uint32_t batchSize);

        void fit(float **input, float **desire, uint32_t batchSize, uint32_t n);

        void fitAll(float **input, float **desire, uint32_t batchSize);

        void fitAll(float **input, float **desire, uint32_t batchSize, uint32_t n);

        void enrich();

        // Forward Computing of the best individual;
        void compute(float *input, float *output);

        // Turn on memory-limit.
        void openMemLimit(uint32_t size);

        // Turn off memory-limit.
        void closeMemLimit();

        // Import the individual.
        static Lycoris *importLycoris(std::string path, uint32_t capacity);

        // Export the individual. The reason why using string to store
        // data is that I just don't wanna use binary.
        void exportLycoris(std::string path);

        // Set p1 to p6 in Args.
        void setMutateArgs(float *p);

        // Set the odds of mutating.
        void setMutateOdds(float odds);

        // Set the numbers of CPU used in project.
        void setCpuCores(uint32_t num);

        // Set the learning rate.
        void setLR(float lr);

        // Get the size of the best individual.
        uint32_t getSize();

        uint32_t getInputNum();

        uint32_t getOutputNum();

        uint32_t getCapacity();

        std::string getMode();

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
        void runLycoris(uint32_t n);

        void addHiddenNodes(uint32_t num);

        void addHiddenNodesCore(uint32_t start, uint32_t end, uint32_t num);

        void addConnections(uint32_t num);

        void addConnectionsCore(uint32_t start, uint32_t end, uint32_t num);

        void checkFirstRun();
    };

}

#endif //LYCORIS_LYCORIS_H
