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
