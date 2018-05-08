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

    std::vector<std::vector<Individual *>> tempList1;
    std::vector<std::vector<unsigned>> tempList2;
    unsigned specieLength;
    std::vector<unsigned> oldLength;

    void (*forwardFuncs)(Individual &in);

    Lycoris(unsigned capacity, unsigned inputNum, unsigned outputNum);

    ~Lycoris();

    std::string version();

    void setMaxTock(unsigned num);

    void setForwardFunc(void (*forwardFuncs)(Individual &in));

    void setGapLength(unsigned num);

    void mate();

    void mateCore(unsigned *start, unsigned *end);

    void mutate();

    void mutateCore(unsigned *start, unsigned *end);

    void classify();

    void classifyCore(unsigned *start, unsigned *end);
};

#endif //LYCORIS_LYCORIS_H
