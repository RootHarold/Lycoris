#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include <vector>
#include <string>
#include "species.h"
#include "utils.h"

class Lycoris {
public:
    Individual *best;
    unsigned capacity;
    unsigned inputNum;
    unsigned outputNum;
    std::vector<Species *> *speciesList;
    Args *args;

    void (*forwardFuncs)(Individual &in);

    Lycoris(unsigned capacity, unsigned inputNum, unsigned outputNum);

    ~Lycoris();

    std::string version();

    void setMaxTock(unsigned num);

    void setForwardFunc(void (*forwardFuncs)(Individual &in));

    void setGapLength(unsigned num);

    void mate();
};

#endif //LYCORIS_LYCORIS_H
