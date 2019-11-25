/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#include "args.h"
#include "utils.h"

namespace LycorisNet {

    Args::Args() {
        utils = new LycorisUtils();

        p1 = 0.05;
        p2 = 0.05;
        p3 = 0.85;
        p4 = 0.05;
        p1B = p1;
        p2B = p2;
        p3B = p3;
        p4B = p4;

        mutateOdds = 31;

        cpuNum = std::thread::hardware_concurrency();

        lr = 0.01;
        mode = "";
        memOverFlag = false;
        memLimitFlag = false;
        firstRun = true;
        batchFlag = true;
    }

    Args::~Args() {
        delete utils;

        for (uint32_t i = 0; i < batchSize; ++i) {
            delete[] batchData[i];
        }
        delete[] batchData;

        delete[] midData;
    }

}
