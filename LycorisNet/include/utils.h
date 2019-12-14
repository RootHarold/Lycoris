/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_UTILS_H
#define LYCORIS_UTILS_H

#include <random>
#include <regex>
#include "individual.h"

namespace LycorisNet {

    /*
     * It is used to assist in sorting individual (neural networks).
     */

    class SortFitness {
    public:
        SortFitness();

        SortFitness(float fitness, uint32_t individualNum);

        ~SortFitness();

        friend class LycorisUtils;

        friend class Lycoris;

    private:
        // Cross entropy or Euclidean distance
        float fitness;
        // The number of neural network
        uint32_t individualNum;
    };

    class Individual;

    // Including some tools.
    class LycorisUtils {
    public:
        LycorisUtils();

        ~LycorisUtils();

        friend class Lycoris;

        friend class Individual;

        friend class Args;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

    private:
        // The engine to emerge random numbers.
        std::random_device rd;

        // ReLU (with huge leak = 0.2).
        inline static float relu(float f) {
            return f > 0 ? f : f * 0.2f;
        }

        // Softmax.
        inline static void softmax(float *array, uint32_t length) {
            float sum = 0;

            for (uint32_t i = 0; i < length; ++i) {
                array[i] = expf(array[i]);
                sum += array[i];
            }

            for (uint32_t i = 0; i < length; ++i) {
                array[i] /= sum;
            }
        }

        // The euclidean distance.
        inline static float euclidean_distance(float *target, std::vector<float> &desire, uint32_t length) {
            float ret = 0;

            for (uint32_t i = 0; i < length; ++i) {
                ret += powf(target[i] - desire[i], 2);
            }

            return sqrtf(ret);
        }

        // Cross-Entropy.
        inline static float cross_entropy(std::vector<float> &p, float *q, uint32_t length) {
            float ret = 0;

            for (uint32_t i = 0; i < length; ++i) {
                ret += p[i] * log2f(q[i] > 0.000001 ? q[i] : 0.000001);
            }

            return ret;
        }

        // Generate a random number of min-max (float).
        inline float LycorisRandomFloat(float min, float max) {
            auto temp =
                    (rd() - std::random_device::min()) / double(std::random_device::max() - std::random_device::min());
            return temp * (max - min) + min;
        };

        // Generate a random number of 0-N (uint32_t).
        inline uint32_t LycorisRandomUint32_t(uint32_t N) {
            return uint32_t(rd() % N);
        }

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

        static void addHiddenNodes(Individual &in, uint32_t num);

        void addConnections(Individual &in, uint32_t num);
    };

}

#endif //LYCORIS_UTILS_H
