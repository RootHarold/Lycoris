/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include "args.h"
#include "utils.h"

namespace LycorisNet {

    Args::Args() {
        utils = new LycorisUtils();

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
        gapList = new std::vector<float>();
        maxGap = 64;
        memOverFlag = false;
        memLimitFlag = false;
        firstOver = false;
        gapListFlag = true;
        firstRun = true;

        checkFlag = false;
        emergeTick = 0;
        emergeTock = 1;
        maxEmergeTock = 16;
        miss = 0;
        hit = 0;
        slopeTick = 2;
        maxSlopeTick = uint32_t(std::exp2(uint32_t(std::log(maxGap) / std::log(2)) - 1));
    }

    Args::~Args() {
        delete utils;
        delete gapList;
    }

}
