#ifndef LYCORIS_FACTORY_H
#define LYCORIS_FACTORY_H

#include <random>
#include "individual.h"

static std::random_device rd;

inline float LycorisRandomFloat(float min, float max) {
    auto temp = (rd() - rd.min()) / float(rd.max() - rd.min() + 1.0);
    return temp * (max - min) + min;
};

inline unsigned LycorisRandomUnsigned(unsigned N) {
    return unsigned(rd() % N);
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

inline float *softmax(float *input, unsigned length) {
    float *ret = new float[length];
    float sum = 0;

    for (unsigned i = 0; i < length; ++i) {
        float temp = float(exp(double(input[i])));
        ret[i] = temp;
        sum += temp;
    }

    for (unsigned i = 0; i < length; ++i) {
        ret[i] /= sum;
    }

    return ret;
}

float distance(Individual &in1, Individual &in2);

unsigned sort1(Individual &in, float *ret1, int *ret2);

#endif //LYCORIS_FACTORY_H
