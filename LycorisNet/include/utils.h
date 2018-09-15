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

        SortFitness(float fitness, uint32_t specieNum, uint32_t individualNum);

        ~SortFitness();

        friend class LycorisUtils;

        friend class Lycoris;

    private:
        float fitness;
        uint32_t specieNum;
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
                ret += powf(target[i] - desire[i]);
            }

            return sqrtf(ret);
        }

        // Return the version information.
        inline static std::string version() {
            return "Lycoris Core 1.9 (CPU Mode)";
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

        // Linear regression. Return the slope.
        inline static float slope(std::vector<float> &y) {
            auto length = y.size();
            std::vector<float> x(length);
            for (uint32_t i = 0; i < length; ++i) {
                x[i] = i;
            }
            auto temp1 = std::accumulate(x.begin(), x.end(), 0.0);
            auto temp2 = std::accumulate(y.begin(), y.end(), 0.0);
            auto temp3 = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
            auto temp4 = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);
            auto ret = float((length * temp4 - temp1 * temp2) / (length * temp3 - temp1 * temp1));
            return ret;
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

        // The distance between two different individuals.
        static float distance(Individual &in1, Individual &in2);

        // Mating two different individuals.
        Individual *mateIndividual(Individual &in1, Individual &in2);

        // Mutating the individual.
        Individual *mutateIndividual(Individual &in);

        // Used in "distance(...)".
        static uint32_t sort1(Individual &in, std::vector<float> &ret1, std::vector<uint32_t> &ret2);

        // Used in "mateIndividual(...)".
        static uint32_t sort2(Individual &in, std::vector<Gen> &ret1, std::vector<Ome> &ret2);
    };

}

#endif //LYCORIS_FACTORY_H
