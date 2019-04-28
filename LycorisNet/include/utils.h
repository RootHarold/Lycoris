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

namespace LycorisNet {

    // Used when sorting individuals.
    class SortFitness {
    public:
        SortFitness();

        SortFitness(float fitness, uint32_t individualNum);

        ~SortFitness();

        friend class LycorisUtils;

        friend class Lycoris;

    private:
        float fitness;
        uint32_t individualNum;
    };

    class Individual;

    // Including some tools.
    class LycorisUtils {
    public:
        LycorisUtils();

        ~LycorisUtils();

        // Sigmoid.
        inline static float sigmoid(float f) {
            return 1 / (1 + expf(-f));
        }

        // Tanh.
        inline static float tanh(float f) {
            return (expf(f) - expf(-f)) / (expf(f) + expf(-f));
        }

        // ReLU (with leak = 0.01).
        inline static float relu(float f) {
            return f > 0 ? f : f * 0.01f;
        }

        // Softmax.
        inline static void softmax(float *input, float *output, uint32_t length) {
            float sum = 0;

            for (uint32_t i = 0; i < length; ++i) {
                auto temp = expf(input[i]);
                output[i] = temp;
                sum += temp;
            }

            for (uint32_t i = 0; i < length; ++i) {
                output[i] /= sum;
            }
        }

        // The euclidean distance.
        inline static float euclidean_distance(float *target, float *desire, uint32_t length) {
            float ret = 0;

            for (uint32_t i = 0; i < length; ++i) {
                ret += powf(target[i] - desire[i], 2);
            }

            return sqrtf(ret);
        }

        // Cross-Entropy.
        inline static float cross_entropy(float *p, float *q, uint32_t length) {
            float ret = 0;

            for (uint32_t i = 0; i < length; ++i) {
                ret -= p[i] * log2f(1 / (q[i] > 0.000001 ? q[i] : 0.000001));
            }

            return ret;
        }

        // Return the version information.
        inline static std::string version() {
            return "Lycoris Core 1.10.1 (CPU Mode)";
        }

        friend class Lycoris;

        friend class Individual;

    private:
        // The engine to emerge random numbers.
        std::random_device rd;

        // Generate a random number of min-max (float).
        inline float LycorisRandomFloat(float min, float max) {
            auto temp = (rd() - std::random_device::min()) /
                        float(std::random_device::max() - std::random_device::min() + 1.0);
            return temp * (max - min) + min;
        };

        // Generate a random number of 0-N (uint32_t).
        inline uint32_t LycorisRandomUint32_t(uint32_t N) {
            return uint32_t(rd() % N);
        };

        inline static std::vector<std::string> split(const std::string &subject) {
            static const std::regex re{"\\s+"};

            std::vector<std::string> container{
                    std::sregex_token_iterator(subject.begin(), subject.end(), re, -1),
                    std::sregex_token_iterator()
            };

            return container;
        }

        // Used with the "SortFitness" when sorting individuals.
        inline static bool compareFitness(const SortFitness &a, const SortFitness &b) {
            return a.fitness < b.fitness;
        }

        // Mutating the individual.
        Individual *mutateIndividual(Individual &in);
    };

}

#endif //LYCORIS_FACTORY_H
