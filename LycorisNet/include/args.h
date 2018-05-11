#ifndef LYCORIS_ARGS_H
#define LYCORIS_ARGS_H

#include <string>
#include <vector>
#include <thread>
#include <climits>

class Args {
public:
    Args();

    ~Args();

    float c1;
    float c2;
    float p1;
    float p2;
    float p3;
    float p4;
    float p5;
    float p6;
    std::string activateFuncName;

    uint32_t mutateTime;
    uint32_t cpuNum;
    float mateOdds;
    float mutateOdds;
    uint32_t maxMutateTime;
    float distanceThreshold;
    float weightA;
    float weightB;
    float biasA;
    float biasB;

    float cleanOdds;

    float (*activateFunc)(float);

    uint32_t tock;
    std::vector<float> *gapList;
    uint32_t maxTock;
    uint32_t maxGap;
};

#endif //LYCORIS_ARGS_H
