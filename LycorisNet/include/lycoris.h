#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include "individual.h"

class Lycoris {
public:
    Individual **individualList;
    unsigned capacity;

    Lycoris();

    Lycoris(unsigned capacity);

    ~Lycoris();
};

#endif //LYCORIS_LYCORIS_H
