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
    return "Lycoris core 1.8-dev-15";
}

void Lycoris::setForwardFunc(void (*forwardFunc)(Individual &)) {
    this->forwardFunc = forwardFunc;
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

void Lycoris::forward() {
    specieLength = uint32_t(speciesList->size());
    auto start = new uint32_t *[args->cpuNum];
    auto end = new uint32_t *[args->cpuNum];
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        start[i] = new uint32_t[specieLength];
        end[i] = new uint32_t[specieLength];
    }

    for (uint32_t i = 0; i < specieLength; ++i) {
        auto individualLength = uint32_t((*speciesList)[i]->individualList->size());
        auto part = individualLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t j = 0; j < temp; ++j) {
            start[j][i] = j * part;
            end[j][i] = (j + 1) * part;
        }
        start[temp][i] = temp * part;
        end[temp][i] = individualLength;
    }

    std::vector<std::thread *> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        std::thread th(&Lycoris::forwardCore, this, start[i], end[i]);
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

void Lycoris::forwardCore(uint32_t *start, uint32_t *end) {
    for (uint32_t i = 0; i < specieLength; ++i) {
        for (uint32_t j = start[i]; j < end[i]; ++j) {
            forwardFunc(*((*(*speciesList)[i]->individualList)[j]));
        }
    }
}

void Lycoris::emergeArgs() {
    if (args->memOverFlag) {
        auto mutateTimeE = LycorisRandomUint32_t(args->maxMutateTime) + 1;
        auto mateOddsE = LycorisRandomFloat(0, 1);
        auto mutateOddsE = LycorisRandomFloat(0, 1);
        float remain = 1;
        float p1E = 0;
        remain -= p1E;
        auto p2E = LycorisRandomFloat(0, 1) * remain;
        remain -= p2E;
        float p3E = 0;
        remain -= p3E;
        auto p4E = LycorisRandomFloat(0, 1) * remain;
        remain -= p4E;
        float p5E = 0;
        remain -= p5E;
        auto p6E = remain;

        if (args->firstOver) {
            args->firstOver = false;
            args->p1B = 0;
            args->p2B = p2E;
            args->p3B = 0;
            args->p4B = p4E;
            args->p5B = 0;
            args->p6B = p6E;
            args->mateOddsB = args->mateOdds;
            args->mutateOddsB = args->mutateOdds;
            args->mutateTimeB = args->mutateTime;
            args->p1 = p1E;
            args->p2 = p2E;
            args->p3 = p3E;
            args->p4 = p4E;
            args->p5 = p5E;
            args->p6 = p6E;
            args->mateOdds = mateOddsE;
            args->mutateOdds = mutateOddsE;
            args->mutateTime = mutateTimeE;
        } else {
            args->p1B = args->p1;
            args->p2B = args->p2;
            args->p3B = args->p3;
            args->p4B = args->p4;
            args->p5B = args->p5;
            args->p6B = args->p6;
            args->mateOddsB = args->mateOdds;
            args->mutateOddsB = args->mutateOdds;
            args->mutateTimeB = args->mutateTime;
            args->p1 = p1E;
            args->p2 = p2E;
            args->p3 = p3E;
            args->p4 = p4E;
            args->p5 = p5E;
            args->p6 = p6E;
            args->mateOdds = mateOddsE;
            args->mutateOdds = mutateOddsE;
            args->mutateTime = mutateTimeE;
        }
    } else {
        auto mutateTimeE = LycorisRandomUint32_t(args->maxMutateTime) + 1;
        auto mateOddsE = LycorisRandomFloat(0, 1);
        auto mutateOddsE = LycorisRandomFloat(0, 1);
        float remain = 1;
        auto p1E = LycorisRandomFloat(0, 1);
        remain -= p1E;
        auto p2E = LycorisRandomFloat(0, 1) * remain;
        remain -= p2E;
        auto p3E = LycorisRandomFloat(0, 1) * remain;
        remain -= p3E;
        auto p4E = LycorisRandomFloat(0, 1) * remain;
        remain -= p4E;
        auto p5E = LycorisRandomFloat(0, 1) * remain;
        remain -= p5E;
        auto p6E = remain;

        args->p1B = args->p1;
        args->p2B = args->p2;
        args->p3B = args->p3;
        args->p4B = args->p4;
        args->p5B = args->p5;
        args->p6B = args->p6;
        args->mateOddsB = args->mateOdds;
        args->mutateOddsB = args->mutateOdds;
        args->mutateTimeB = args->mutateTime;
        args->p1 = p1E;
        args->p2 = p2E;
        args->p3 = p3E;
        args->p4 = p4E;
        args->p5 = p5E;
        args->p6 = p6E;
        args->mateOdds = mateOddsE;
        args->mutateOdds = mutateOddsE;
        args->mutateTime = mutateTimeE;
    }
}

void Lycoris::autoParameter() {

}

void Lycoris::chooseElite() {
    uint32_t totalLength = 0;
    for (auto iter = speciesList->begin(); iter < speciesList->end(); ++iter) {
        totalLength += (*iter)->individualList->size();
        // TODO
    }
}
