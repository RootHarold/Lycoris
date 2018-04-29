#ifndef LYCORIS_FACTORY_H
#define LYCORIS_FACTORY_H

#include <random>

static std::random_device rd;

inline float LycorisRandomFloat(float min, float max) {
    auto temp = (rd() - rd.min()) / float(rd.max() - rd.min() + 1.0);
    return temp * (max - min) + min;
};

inline unsigned LycorisRandomUnsigned(unsigned N) {
    return unsigned(rd() % N);
};

inline float sigmoid(float f) {
    return float(1 / (1 + exp(double(0 - f))));
}

#endif //LYCORIS_FACTORY_H
