#include "lycoris.h"

Lycoris::Lycoris(unsigned capacity, unsigned inputNum, unsigned outputNum) {
    this->capacity = capacity;
    this->inputNum = inputNum;
    this->outputNum = outputNum;
    args = new Args();
}

Lycoris::~Lycoris() {
    delete args;

    for (auto iter = speciesList->begin(); iter != speciesList->end(); ++iter) {
        delete *iter;
    }

    delete speciesList;
}

std::string Lycoris::version() {
    return "Lycoris core 1.8-dev-10";
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
    specieLength = unsigned(speciesList->size());
    tempList1 = new Individual **[specieLength];
    oldLength = new unsigned[specieLength];
    auto start = new unsigned *[args->cpuNum];
    auto end = new unsigned *[args->cpuNum];
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        start[i] = new unsigned[specieLength];
        end[i] = new unsigned[specieLength];
    }

    for (unsigned i = 0; i < specieLength; ++i) {
        auto individualLength = unsigned((*speciesList)[i]->individualList->size());
        oldLength[i] = individualLength;
        auto mateLength = unsigned(float(individualLength) * args->mateOdds);
        tempList1[i] = new Individual *[mateLength];
        auto part = mateLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (unsigned j = 0; j < temp; ++j) {
            start[j][i] = j * part;
            end[j][i] = (j + 1) * part;
        }
        start[temp][i] = temp * part;
        end[temp][i] = mateLength;
    }

    std::vector<std::thread *> threads;
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        std::thread th(mateCore, start[i], end[i]);
        threads.push_back(&th);
    }
    for (std::thread *i :threads) {
        i->join();
    }

    for (unsigned i = 0; i < specieLength; ++i) {
        delete[] tempList1[i];
    }
    delete[] tempList1;
    delete[] oldLength;
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        delete[] start[i];
        delete[] end[i];
    }
    delete[] start;
    delete[] end;
}

void Lycoris::mateCore(unsigned *start, unsigned *end) {
    for (unsigned i = 0; i < specieLength; ++i) {
        auto l = (*speciesList)[i]->individualList;
        for (unsigned j = start[i]; j < end[i]; ++j) {
            tempList1[i][j] = mateIndividual(*((*l)[LycorisRandomUnsigned(oldLength[i])]),
                                             *((*l)[LycorisRandomUnsigned(oldLength[i])]));
        }
    }
}

void Lycoris::mutate() {
    specieLength = unsigned(speciesList->size());
    tempList1 = new Individual **[specieLength];
    oldLength = new unsigned[specieLength];
    auto start = new unsigned *[args->cpuNum];
    auto end = new unsigned *[args->cpuNum];
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        start[i] = new unsigned[specieLength];
        end[i] = new unsigned[specieLength];
    }

    for (unsigned i = 0; i < specieLength; ++i) {
        auto individualLength = unsigned((*speciesList)[i]->individualList->size());
        oldLength[i] = individualLength;
        auto mutateLength = unsigned(float(individualLength) * args->mutateOdds);
        tempList1[i] = new Individual *[mutateLength];
        auto part = mutateLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (unsigned j = 0; j < temp; ++j) {
            start[j][i] = j * part;
            end[j][i] = (j + 1) * part;
        }
        start[temp][i] = temp * part;
        end[temp][i] = mutateLength;
    }

    std::vector<std::thread *> threads;
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        std::thread th(mutateCore, start[i], end[i]);
        threads.push_back(&th);
    }
    for (std::thread *i :threads) {
        i->join();
    }

    for (unsigned i = 0; i < specieLength; ++i) {
        delete[] tempList1[i];
    }
    delete[] tempList1;
    delete[] oldLength;
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        delete[] start[i];
        delete[] end[i];
    }
    delete[] start;
    delete[] end;
}

void Lycoris::mutateCore(unsigned *start, unsigned *end) {
    for (unsigned i = 0; i < specieLength; ++i) {
        auto l = (*speciesList)[i]->individualList;
        for (unsigned j = start[i]; j < end[i]; ++j) {
            tempList1[i][j] = mutateIndividual(*((*l)[LycorisRandomUnsigned(oldLength[i])]));
        }
    }
}

void Lycoris::classify() {
    tempList2 = new unsigned *[specieLength];
    auto start = new unsigned *[args->cpuNum];
    auto end = new unsigned *[args->cpuNum];
    for (unsigned i = 0; i < args->cpuNum; ++i) {
        start[i] = new unsigned[specieLength];
        end[i] = new unsigned[specieLength];
    }

    for (unsigned i = 0; i < specieLength; ++i) {
        // TODO
    }
}

void Lycoris::classifyCore(unsigned *start, unsigned *end) {

}
