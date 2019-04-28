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

        p1 = 0.2;
        p2 = 0.1;
        p3 = 0.2;
        p4 = 0.1;
        p5 = 0.2;
        p6 = 0.2;
        p1B = p1;
        p2B = p2;
        p3B = p3;
        p4B = p4;
        p5B = p5;
        p6B = p6;
        activateFuncName = "sigmoid";

        mutateOdds = 1.08;
        weightA = -1;
        weightB = 1;
        biasA = -1;
        biasB = 1;

        cpuNum = std::thread::hardware_concurrency();

        cleanOdds = 0.01;
        activateFunc = LycorisUtils::sigmoid;
        memOverFlag = false;
        memLimitFlag = false;
        firstRun = true;
    }

    Args::~Args() {
        delete utils;
    }

}
