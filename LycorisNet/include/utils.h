#ifndef LYCORIS_FACTORY_H
#define LYCORIS_FACTORY_H

#include <random>

static std::random_device rd;

inline float LycorisRandomFloat() {
    return (rd() - rd.min()) / float(rd.max() - rd.min());
};

inline unsigned LycorisRandomUnsigned(unsigned N) {
    return unsigned(LycorisRandomFloat() * N);
};

#endif //LYCORIS_FACTORY_H
