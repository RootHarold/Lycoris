/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#include "args.h"
#include "utils.h"

namespace LycorisNet {

    /*
     * Preset some hyper-parameters.
     */

    Args::Args() {
        utils = new LycorisUtils();

        p1 = 0.3;
        p2 = 0.075;
        p3 = 0.5;
        p4 = 0.125;
        p1B = p1;
        p2B = p2;
        p3B = p3;
        p4B = p4;

        mutateOdds = 15;
        cpuNum = std::thread::hardware_concurrency();
        lr = 0.01;

        memOverFlag = false;
        memLimitFlag = false;
        firstRun = true;
        batchFlag = true;
        preheatFlag = true;
        evolveFlag = true;
    }

    Args::~Args() {
        delete utils;

        if (!batchFlag) {
            for (uint32_t i = 0; i < batchSize_; ++i) {
                delete[] batchData[i];
            }
            delete[] batchData;

            delete[] midData;
        }
    }

}
