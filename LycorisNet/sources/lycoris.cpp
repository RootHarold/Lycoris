#include "lycoris.h"

Lycoris::Lycoris(unsigned capacity, unsigned inputNum, unsigned outputNum) {
    this->capacity = capacity;
    this->inputNum = inputNum;
    this->outputNum = outputNum;
    args = new Args();
    args->tock = 1;
}

Lycoris::~Lycoris() {
    delete args;

    for (auto iter = speciesList->begin(); iter != speciesList->end(); ++iter) {
        delete *iter;
    }

    delete speciesList;
}

std::string Lycoris::version() {
    return "Lycoris core 1.8-dev-8";
}

void Lycoris::setForwardFunc(void (*forwardFuncs)(Individual &)) {
    this->forwardFuncs = forwardFuncs;
}

void Lycoris::setMaxTock(unsigned num) {
    args->maxTock = num;
}

void Lycoris::setGapLength(unsigned num) {
    args->maxGap = num - 1;
}

void Lycoris::mate() {
    // TODO
}
