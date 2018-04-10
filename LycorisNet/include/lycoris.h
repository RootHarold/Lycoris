#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include "individual.h"
#include <string>

class Lycoris {
public:
    Individual **individualList;
    unsigned capacity;

    Lycoris();

    Lycoris(unsigned capacity);

    ~Lycoris();

    std::string version();
};

#endif //LYCORIS_LYCORIS_H
