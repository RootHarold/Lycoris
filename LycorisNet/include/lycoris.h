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

        // Set the weightRandom()'s range.
        void setWeigthRandom(float a, float b);

        // Set the biasRandom()'s range.
        void setBiasRandom(float a, float b);

        // Import the individual.
        static Lycoris *importLycoris(std::string path, uint32_t capacity);

        // Export the individual. The reason why using string to store
        // data is that I just don't wanna use binary.
        void exportLycoris(std::string path);

        // Set p1 to p6 in Args.
        void setMutateArgs(float *p);

        // Set the numbers of CPU used in project.
        void setCpuCores(uint32_t num);

    private:
        // An object of Args is integrated into this.
        Args *args;

        std::vector<Individual *> *individualList;
        std::vector<Individual *> tempList;
        // Store the length of individualList in speciesList.
        uint32_t oldLength;

        // This core function needs to be completed.
        void (*forwardFunc)(Individual &in);

        // Mutating.
        void mutate();

        // The parallel kernel of mutate().
        void mutateCore(uint32_t start, uint32_t end);

        // All individuals are calculated forward.
        void forward();

        // The parallel kernel of forward().
        void forwardCore(uint32_t start, uint32_t end);

        // Choose elites and manipulate gapList.
        void chooseElite();
    };

}

#endif //LYCORIS_LYCORIS_H
