/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include "genome.h"

Gen::Gen() {

}

Gen::Gen(uint32_t in, uint32_t out) {
    this->in = in;
    this->out = out;
}

Ome::Ome() {

}

Ome::Ome(float weight, bool isEnable, uint32_t innovationNum) {
    this->weight = weight;
    this->isEnable = isEnable;
    this->innovationNum = innovationNum;
}
