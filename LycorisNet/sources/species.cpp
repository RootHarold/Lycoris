/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include "species.h"

Species::Species() {
    individualList = new std::vector<Individual *>();
}

Species::~Species() {
    for (auto iter = individualList->begin(); iter != individualList->end(); ++iter) {
        delete *iter;
    }

    delete individualList;
}
