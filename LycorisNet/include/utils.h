#ifndef LYCORIS_FACTORY_H
#define LYCORIS_FACTORY_H

#include <random>
#include "individual.h"

static std::random_device rd;

inline float LycorisRandomFloat(float min, float max) {
    auto temp = (rd() - rd.min()) / float(rd.max() - rd.min() + 1.0);
    return temp * (max - min) + min;
};

inline uint32_t LycorisRandomUint32_t(uint32_t N) {
    return uint32_t(rd() % N);
};

inline float sigmoid(float f) {
    return float(1 / (1 + exp(double(-f))));
}

inline float tanh(float f) {
    return float((exp(double(f)) - exp(double(-f))) / (exp(double(f)) + exp(double(-f))));
}

inline float relu(float f) {
    return f > 0 ? f : f * 0.01f;
}

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

float distance(Individual &in1, Individual &in2);

uint32_t sort1(Individual &in, float *ret1, int *ret2);

uint32_t sort2(Individual &in, Gen *ret1, Ome *ret2);

Individual *mateIndividual(Individual &in1, Individual &in2);

Individual *mutateIndividual(Individual &in);

class SortFitness {
public:
    float fitness;
    uint32_t specieNum;
    uint32_t individualNum;

    SortFitness();

    SortFitness(float fitness, uint32_t specieNum, uint32_t individualNum);

    ~SortFitness();
};

bool compareFitness(const SortFitness &a, const SortFitness &b);

#endif //LYCORIS_FACTORY_H
