#include "lycoris.h"

Lycoris::Lycoris(uint32_t capacity, uint32_t inputNum, uint32_t outputNum) {
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
    return "Lycoris core 1.8-dev-14";
}

void Lycoris::setForwardFunc(void (*forwardFuncs)(Individual &)) {
    this->forwardFuncs = forwardFuncs;
}

void Lycoris::setMaxTock(uint32_t num) {
    args->maxTock = num;
}

void Lycoris::setGapLength(uint32_t num) {
    args->maxGap = num - 1;
}

void Lycoris::setDistanceThreshold(float threshold) {
    args->distanceThreshold = threshold;
}

void Lycoris::mate() {
    specieLength = uint32_t(speciesList->size());
    tempList1.resize(specieLength);
    oldLength.resize(specieLength);
    auto start = new uint32_t *[args->cpuNum];
    auto end = new uint32_t *[args->cpuNum];
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        start[i] = new uint32_t[specieLength];
        end[i] = new uint32_t[specieLength];
    }

    for (uint32_t i = 0; i < specieLength; ++i) {
        auto individualLength = uint32_t((*speciesList)[i]->individualList->size());
        oldLength[i] = individualLength;
        auto mateLength = uint32_t(float(individualLength) * args->mateOdds);
        tempList1[i].resize(mateLength);
        auto part = mateLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t j = 0; j < temp; ++j) {
            start[j][i] = j * part;
            end[j][i] = (j + 1) * part;
        }
        start[temp][i] = temp * part;
        end[temp][i] = mateLength;
    }

    std::vector<std::thread *> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        std::thread th(&Lycoris::mateCore, this, start[i], end[i]);
        threads.push_back(&th);
    }
    for (std::thread *i :threads) {
        i->join();
    }

    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        delete[] start[i];
        delete[] end[i];
    }
    delete[] start;
    delete[] end;
}

void Lycoris::mateCore(uint32_t *start, uint32_t *end) {
    for (uint32_t i = 0; i < specieLength; ++i) {
        auto l = (*speciesList)[i]->individualList;
        for (uint32_t j = start[i]; j < end[i]; ++j) {
            tempList1[i][j] = mateIndividual(*((*l)[LycorisRandomUint32_t(oldLength[i])]),
                                             *((*l)[LycorisRandomUint32_t(oldLength[i])]));
        }
    }
}

void Lycoris::mutate() {
    specieLength = uint32_t(speciesList->size());
    tempList1.resize(specieLength);
    oldLength.resize(specieLength);
    auto start = new uint32_t *[args->cpuNum];
    auto end = new uint32_t *[args->cpuNum];
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        start[i] = new uint32_t[specieLength];
        end[i] = new uint32_t[specieLength];
    }

    for (uint32_t i = 0; i < specieLength; ++i) {
        auto individualLength = uint32_t((*speciesList)[i]->individualList->size());
        oldLength[i] = individualLength;
        auto mutateLength = uint32_t(float(individualLength) * args->mutateOdds);
        tempList1[i].resize(mutateLength);
        auto part = mutateLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t j = 0; j < temp; ++j) {
            start[j][i] = j * part;
            end[j][i] = (j + 1) * part;
        }
        start[temp][i] = temp * part;
        end[temp][i] = mutateLength;
    }

    std::vector<std::thread *> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        std::thread th(&Lycoris::mutateCore, this, start[i], end[i]);
        threads.push_back(&th);
    }
    for (std::thread *i :threads) {
        i->join();
    }

    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        delete[] start[i];
        delete[] end[i];
    }
    delete[] start;
    delete[] end;
}

void Lycoris::mutateCore(uint32_t *start, uint32_t *end) {
    for (uint32_t i = 0; i < specieLength; ++i) {
        auto l = (*speciesList)[i]->individualList;
        for (uint32_t j = start[i]; j < end[i]; ++j) {
            tempList1[i][j] = mutateIndividual(*((*l)[LycorisRandomUint32_t(oldLength[i])]));
        }
    }
}

void Lycoris::classify() {
    tempList2.resize(specieLength);
    auto start = new uint32_t *[args->cpuNum];
    auto end = new uint32_t *[args->cpuNum];
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        start[i] = new uint32_t[specieLength];
        end[i] = new uint32_t[specieLength];
    }

    for (uint32_t i = 0; i < specieLength; ++i) {
        auto tempList1Length = uint32_t(tempList1[i].size());
        tempList2[i].resize(tempList1Length);
        auto part = tempList1Length / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t j = 0; j < temp; ++j) {
            start[j][i] = j * part;
            end[j][i] = (j + 1) * part;
        }
        start[temp][i] = temp * part;
        end[temp][i] = tempList1Length;
    }

    std::vector<std::thread *> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        std::thread th(&Lycoris::classifyCore, this, start[i], end[i]);
        threads.push_back(&th);
    }
    for (std::thread *i :threads) {
        i->join();
    }

    speciesList->push_back(new Species());
    for (uint32_t i = 0; i < tempList2.size(); ++i) {
        for (uint32_t j = 0; j < tempList2[i].size(); ++j) {
            if (tempList2[i][j] != UINT32_MAX) {
                (*speciesList)[tempList2[i][j]]->individualList->push_back(tempList1[i][j]);
            } else {
                (*speciesList)[specieLength]->individualList->push_back(tempList1[i][j]);
            }
        }
    }

    for (auto iter = speciesList->begin(); iter != speciesList->end();) {
        if ((*iter)->individualList->empty()) {
            iter = speciesList->erase(iter);
        } else {
            ++iter;
        }
    }

    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        delete[] start[i];
        delete[] end[i];
    }
    delete[] start;
    delete[] end;
}

void Lycoris::classifyCore(uint32_t *start, uint32_t *end) {
    for (uint32_t i = 0; i < specieLength; ++i) {
        for (uint32_t j = start[i]; j < end[i]; ++j) {
            auto temp = tempList1[i][j];
            for (uint32_t k = 0; k < specieLength; ++k) {
                auto l = (*speciesList)[k]->individualList;
                auto dis = distance(*temp, *((*l)[LycorisRandomUint32_t(oldLength[k])]));
                if (dis <= args->distanceThreshold) {
                    tempList2[i][j] = k;
                    break;
                } else {
                    tempList2[i][j] = UINT32_MAX;
                }
            }
        }
    }
}
