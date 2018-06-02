#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include <vector>
#include <string>
#include "species.h"
#include "utils.h"

class Lycoris {
public:
    uint32_t inputNum;
    uint32_t outputNum;
    uint32_t capacity;
    Individual *best;

    Lycoris(uint32_t capacity, uint32_t inputNum, uint32_t outputNum);

    ~Lycoris();

    static std::string version();

    void setForwardFunc(void (*forwardFunc)(Individual &in));

    void setActivateFunc(std::string function);

    void runLycoris();

    void openMemLimit(uint32_t size);

    void closeMemLimit();

    void setMaxTock(uint32_t num);

    void setGapLength(uint32_t num);

    void setDistanceThreshold(float threshold);

    void setDistanceArgs(float a, float b);

    void setMaxMutateTime(uint32_t num);

    void setWeigthRandom(float a, float b);

    void setBiasRandom(float a, float b);

    static Lycoris *importLycoris(std::string path, uint32_t capacity);

    void exportLycoris(std::string path);

private:
    Args *args;
    std::vector<Species *> *speciesList;
    std::vector<std::vector<Individual *>> tempList1;
    std::vector<std::vector<uint32_t >> tempList2;
    uint32_t specieLength;
    std::vector<uint32_t> oldLength;

    void (*forwardFunc)(Individual &in);

    void mate();

    void mateCore(uint32_t *start, uint32_t *end);

    void mutate();

    void mutateCore(uint32_t *start, uint32_t *end);

    void classify();

    void classifyCore(uint32_t *start, uint32_t *end);

    void forward();

    void forwardCore(uint32_t *start, uint32_t *end);

    void emergeArgs();

    void autoParameter();

    void chooseElite();
};

#endif //LYCORIS_LYCORIS_H
