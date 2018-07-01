/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_FACTORY_H
#define LYCORIS_FACTORY_H

#include <random>
#include <regex>
#include "individual.h"

// A global engine to emerge random numbers.
// Need to be refactored elegantly.
static std::random_device rd;

// namespace LycorisUtils
namespace LycorisUtils {

    // Generate a random number of min-max (float).
    inline float LycorisRandomFloat(float min, float max) {
        auto temp = (rd() - rd.min()) / float(rd.max() - rd.min() + 1.0);
        return temp * (max - min) + min;
    };

    // Generate a random number of 0-N (uint32_t).
    inline uint32_t LycorisRandomUint32_t(uint32_t N) {
        return uint32_t(rd() % N);
    };

    // Sigmoid.
    inline float sigmoid(float f) {
        return float(1 / (1 + exp(double(-f))));
    }

    // Tanh.
    inline float tanh(float f) {
        return float((exp(double(f)) - exp(double(-f))) / (exp(double(f)) + exp(double(-f))));
    }

    // ReLU (with leak = 0.01).
    inline float relu(float f) {
        return f > 0 ? f : f * 0.01f;
    }

    // Softmax.
    inline float *softmax(float *input, uint32_t length) {
        auto *ret = new float[length];
        float sum = 0;

        for (uint32_t i = 0; i < length; ++i) {
            auto temp = float(exp(double(input[i])));
            ret[i] = temp;
            sum += temp;
        }

        for (uint32_t i = 0; i < length; ++i) {
            ret[i] /= sum;
        }

        return ret;
    }

}

inline std::vector<std::string> split(const std::string &subject) {
    static const std::regex re{"\\s+"};

    std::vector<std::string> container{
            std::sregex_token_iterator(subject.begin(), subject.end(), re, -1),
            std::sregex_token_iterator()
    };

    return container;
}

// Used when sorting individuals.
class SortFitness {
public:
    float fitness;
    uint32_t specieNum;
    uint32_t individualNum;

    SortFitness();

    SortFitness(float fitness, uint32_t specieNum, uint32_t individualNum);

    ~SortFitness();
};

inline bool compareFitness(const SortFitness &a, const SortFitness &b) {
    return a.fitness < b.fitness;
}

// The distance between two different individuals.
float distance(Individual &in1, Individual &in2);

// Mating two different individuals.
Individual *mateIndividual(Individual &in1, Individual &in2);

// Mutating the individual.
Individual *mutateIndividual(Individual &in);

// Used in "distance(...)".
uint32_t sort1(Individual &in, float *ret1, int *ret2);

// Used in "mateIndividual(...)".
uint32_t sort2(Individual &in, Gen *ret1, Ome *ret2);

#endif //LYCORIS_FACTORY_H
