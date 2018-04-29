#ifndef LYCORIS_ARGS_H
#define LYCORIS_ARGS_H

#include <string>

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

    unsigned mutateTime;
    unsigned cpuNum;
    float mateOdds;
    float mutateOdds;
    unsigned maxMutateTime;
    float distanceThreshold;
    float weightA;
    float weightB;
    float biasA;
    float biasB;

    float cleanOdds;
};

#endif //LYCORIS_ARGS_H
