#include "args.h"
#include "utils.h"

Args::Args() {
    c1 = 1.0;
    c2 = 0.4;
    p1 = 0.1;
    p2 = 0.2;
    p3 = 0.2;
    p4 = 0.2;
    p5 = 0.1;
    p6 = 0.2;
    activateFuncName = "sigmoid";

    mutateTime = 1;
    mateOdds = 1;
    mutateOdds = 1;
    maxMutateTime = 10;
    weightA = -1;
    weightB = 1;
    biasA = -1;
    biasB = 1;

    cpuNum = std::thread::hardware_concurrency();

    cleanOdds = 0.01;
    activateFunc = LycorisUtils::sigmoid;
    tock = 1;
    gapList = new std::vector<float>();
    maxTock = 16;
    maxGap = 64;
    memOverFlag = false;
    memLimitFlag = false;
    firstOver = false;
    checkFlag = false;
    miss = 0;
    hit = 0;
    tick = 1;
    gapListFlag = true;
    firstRun = true;
}

Args::~Args() {
    delete gapList;
}
