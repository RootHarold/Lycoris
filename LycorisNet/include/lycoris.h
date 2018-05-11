#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include <vector>
#include <string>
#include "species.h"
#include "utils.h"

class Lycoris {
public:
    Individual *best;
    uint32_t capacity;
    uint32_t inputNum;
    uint32_t outputNum;
    std::vector<Species *> *speciesList;
    Args *args;

    std::vector<std::vector<Individual *>> tempList1;
    std::vector<std::vector<uint32_t >> tempList2;
    uint32_t specieLength;
    std::vector<uint32_t> oldLength;

    void (*forwardFuncs)(Individual &in);

    Lycoris(uint32_t capacity, uint32_t inputNum, uint32_t outputNum);

    ~Lycoris();

    std::string version();

    void setMaxTock(uint32_t num);

    void setForwardFunc(void (*forwardFuncs)(Individual &in));

    void setGapLength(uint32_t num);

    void setDistanceThreshold(float threshold);

    void mate();

    void mateCore(uint32_t *start, uint32_t *end);

    void mutate();

    void mutateCore(uint32_t *start, uint32_t *end);

    void classify();

    void classifyCore(uint32_t *start, uint32_t *end);
};

#endif //LYCORIS_LYCORIS_H
