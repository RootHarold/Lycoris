/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include <vector>
#include <string>
#include "species.h"
#include "utils.h"

namespace LycorisNet {

    /*
     * The class Lycoris is the highest level module of LycorisNet.
     */

    class Lycoris {
    public:
        // The dimension of input.
        uint32_t inputNum;
        // The dimension of output.
        uint32_t outputNum;
        // The capacity of Lycoris.
        uint32_t capacity;
        // The best individual.
        Individual *best;

        Lycoris(uint32_t capacity, uint32_t inputNum, uint32_t outputNum);

        ~Lycoris();

        // Apply the core function to lycoris. It is driven by events and
        // promotes the fitting of the whole network.
        void setForwardFunc(void (*forwardFunc)(Individual &in));

        // Set the activate function.
        void setActivateFunc(std::string function);

        // Each time this function is called, the network runs forward one time.
        void runLycoris();

        // Run forward n times.
        void runLycoris(uint32_t n);

        // Turn on memory-limit.
        void openMemLimit(uint32_t size);

        // Turn off memory-limit.
        void closeMemLimit();

        // Set the maximum tock.
        void setMaxTock(uint32_t num);

        // Set the length of gapList.
        void setGapLength(uint32_t num);

        // Set the threshold of the distance between two individuals.
        void setDistanceThreshold(float threshold);

        //Set "c1" and "c2".
        void setDistanceArgs(float a, float b);

        // Set the maximum number of mutating.
        void setMaxMutateTime(uint32_t num);

        // Set the weightRandom()'s range.
        void setWeigthRandom(float a, float b);

        // Set the biasRandom()'s range.
        void setBiasRandom(float a, float b);

        // Import the individual.
        static Lycoris *importLycoris(std::string path, uint32_t capacity);

        // Export the individual. The reason why using string to store
        // data is that I just don't wanna use binary.
        void exportLycoris(std::string path);

    private:
        // An object of Args is integrated into this.
        Args *args;
        // The slice of species.
        std::vector<Species *> *speciesList;
        // A temporary two-dimensional slice to store individuals.
        std::vector<std::vector<Individual *>> tempList1;
        // A temporary slice used in classify().
        std::vector<std::vector<uint32_t >> tempList2;
        // The length of speciesList.
        uint32_t specieLength;
        // Store the length of individualList in speciesList.
        std::vector<uint32_t> oldLength;

        // This core function needs to be completed.
        void (*forwardFunc)(Individual &in);

        // Mating.
        void mate();

        // The parallel kernel of mate().
        void mateCore(uint32_t *start, uint32_t *end);

        // Mutating.
        void mutate();

        // The parallel kernel of mutate().
        void mutateCore(uint32_t *start, uint32_t *end);

        // Computing distances between individuals. And then classify them.
        void classify();

        // The parallel kernel of classify().
        void classifyCore(uint32_t *start, uint32_t *end);

        // All individuals are calculated forward.
        void forward();

        // The parallel kernel of forward().
        void forwardCore(uint32_t *start, uint32_t *end);

        void emergeArgs();

        // Update some parameters automatically.
        void autoParameter();

        // Choose elites and manipulate gapList.
        void chooseElite();
    };

}

#endif //LYCORIS_LYCORIS_H
