#include "lycoris.h"
#include <algorithm>
#include <cfloat>
#include <iostream>

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
    return "Lycoris core 1.8-dev-18";
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

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        threads.emplace_back(std::thread(&Lycoris::mateCore, this, start[i], end[i]));
    }
    for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
        (*iter).join();
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

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        threads.emplace_back(std::thread(&Lycoris::mutateCore, this, start[i], end[i]));
    }
    for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
        (*iter).join();
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

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        threads.emplace_back(std::thread(&Lycoris::classifyCore, this, start[i], end[i]));
    }
    for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
        (*iter).join();
    }

    speciesList->push_back(new Species());
    for (uint32_t i = 0; i < tempList2.size(); ++i) {
        for (uint32_t j = 0; j < tempList2[i].size(); ++j) {
            (*speciesList)[tempList2[i][j]]->individualList->push_back(tempList1[i][j]);
        }
    }

    for (auto iter = speciesList->begin(); iter != speciesList->end();) {
        if ((*iter)->individualList->empty()) {
            delete *iter;
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
                    tempList2[i][j] = specieLength;
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

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < args->cpuNum; ++i) {
        threads.emplace_back(std::thread(&Lycoris::forwardCore, this, start[i], end[i]));
    }
    for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
        (*iter).join();
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
        auto mateOddsE = LycorisRandomFloat(1, 2);
        auto mutateOddsE = LycorisRandomFloat(1, 2);
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
        auto mateOddsE = LycorisRandomFloat(1, 2);
        auto mutateOddsE = LycorisRandomFloat(1, 2);
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
    if (args->checkFlag) {
        auto length = uint32_t(args->gapList->size());
        auto lastValue = args->gapList->back();
        uint32_t count = 0;
        for (auto iter = args->gapList->begin(); iter != args->gapList->end(); ++iter) {
            if (lastValue > (*iter)) {
                count++;
            }
        }

        if (count < (length / 2 + 1)) {
            args->miss++;
            args->hit = 0;
            if (args->miss == 2) {
                if (args->tock > 1) {
                    args->tock /= 2;
                }
                args->miss = 1;
            }

            args->p1 = args->p1B;
            args->p2 = args->p2B;
            args->p3 = args->p3B;
            args->p4 = args->p4B;
            args->p5 = args->p5B;
            args->p6 = args->p6B;
            args->mateOdds = args->mateOddsB;
            args->mutateOdds = args->mutateOddsB;
            args->mutateTime = args->mutateTimeB;
        } else {
            args->hit++;
            args->miss = 0;
            if (args->hit == 2) {
                if (args->tock < args->maxTock) {
                    args->tock *= 2;
                }
                args->hit = 1;
            }
        }
        args->checkFlag = false;
    }

    if (args->tick == args->tock) {
        emergeArgs();
        args->checkFlag = true;
        args->tick = 0;
    } else {
        args->tick += 1;
    }
}

void Lycoris::chooseElite() {
    uint32_t totalLength = 0;
    for (auto iter = speciesList->begin(); iter != speciesList->end(); ++iter) {
        totalLength += (*iter)->individualList->size();
    }

    if (totalLength == 0) {
        std::cout << "All died." << std::endl;
        exit(0);
    }

    std::vector<SortFitness> sortList(totalLength);
    uint32_t pointer = 0;
    for (uint32_t i = 0; i < speciesList->size(); ++i) {
        auto temp = (*speciesList)[i]->individualList;
        for (uint32_t j = 0; j < temp->size(); ++j) {
            if (std::isnan((*temp)[j]->fitness) || std::isinf((*temp)[j]->fitness)) {
                (*temp)[j]->fitness = FLT_MIN;
            }

            sortList[pointer] = SortFitness((*temp)[j]->fitness, i, j);
            pointer++;
        }
    }
    std::sort(sortList.begin(), sortList.end(), compareFitness);
    auto last = sortList[totalLength - 1];
    auto tempBest = best->fitness;
    best = (*(*speciesList)[last.specieNum]->individualList)[last.individualNum];

    auto newSpecieList = new std::vector<Species *>(specieLength);
    for (uint32_t i = 0; i < specieLength; ++i) {
        (*newSpecieList)[i] = new Species();
    }
    auto newLength = uint32_t(float(capacity) / ((1 + args->mateOdds) * (1 + args->mutateOdds)));
    uint32_t memSum = 0;
    auto z = totalLength;
    for (; z > totalLength - newLength; --z) {
        if (z == 0) {
            break;
        }
        auto temp = sortList[z - 1];
        auto tempIndividual = (*(*speciesList)[temp.specieNum]->individualList)[temp.individualNum];
        if (args->memLimitFlag) {
            memSum += tempIndividual->getSize();
        }
        (*newSpecieList)[temp.specieNum]->individualList->push_back(tempIndividual);
    }
    if (args->memLimitFlag) {
        if (memSum > (totalLength - z) * args->limitSize) {
            if (!args->memOverFlag) {
                args->firstOver = true;
                args->memOverFlag = true;
            }
        } else {
            args->memOverFlag = false;
        }
    }

    for (auto iter = newSpecieList->begin(); iter != newSpecieList->end();) {
        if ((*iter)->individualList->empty()) {
            delete *iter;
            iter = newSpecieList->erase(iter);
        } else {
            ++iter;
        }
    }

    if (totalLength - newLength > 0) {
        for (uint32_t i = 0; i < totalLength - newLength; ++i) {
            if (i == totalLength) {
                break;
            }
            auto temp = sortList[i];
            auto tempIndividual = (*(*speciesList)[temp.specieNum]->individualList)[temp.individualNum];
            delete tempIndividual;
        }
    }
    for (auto iter = speciesList->begin(); iter != speciesList->end(); ++iter) {
        (*iter)->individualList->clear();
        delete *iter;
    }
    delete speciesList;
    speciesList = newSpecieList;

    auto difference = best->fitness - tempBest;
    if (args->gapListFlag) {
        auto length = args->gapList->size();
        if (length == args->maxGap) {
            args->gapListFlag = false;
        }
        args->gapList->push_back(difference);
    } else {
        args->gapList->erase(args->gapList->begin());
        args->gapList->push_back(difference);
    }
}

void Lycoris::runLycoris() {
    if (args->firstRun) {
        args->firstRun = false;
        float weightDiff;
        if (args->weightB > args->weightA) {
            weightDiff = args->weightB - args->weightA;
        } else {
            weightDiff = args->weightA - args->weightB;
        }
        args->distanceThreshold = args->c1 * 0.2f + args->c2 * 0.2f * weightDiff;

        auto specie = new Species();
        auto initialCapacity = uint32_t(float(capacity) / ((1 + args->mateOdds) * (1 + args->mutateOdds)));
        specie->individualList->resize(initialCapacity);
        for (unsigned i = 0; i < initialCapacity; ++i) {
            (*(specie->individualList))[i] = new Individual(inputNum, outputNum, args);
        }
        speciesList = new std::vector<Species *>();
        speciesList->push_back(specie);
        best = (*(*speciesList)[0]->individualList)[0];
    }

    mate();
    classify();
    mutate();
    classify();
    forward();
    autoParameter();
    chooseElite();
}

void Lycoris::openMemLimit(uint32_t size) {
    args->limitSize = size * 9 / 10;
    args->memLimitFlag = true;
}

void Lycoris::closeMemLimit() {
    args->memLimitFlag = false;
    args->memOverFlag = false;
    args->firstOver = false;
}

void Lycoris::setDistanceArgs(float a, float b) {
    args->c1 = a;
    args->c2 = b;
}

void Lycoris::setMaxMutateTime(uint32_t num) {
    args->maxMutateTime = num;
}

void Lycoris::setWeigthRandom(float a, float b) {
    args->weightA = a;
    args->weightB = b;
}

void Lycoris::setBiasRandom(float a, float b) {
    args->biasA = a;
    args->biasB = b;
}

void Lycoris::setActivateFunc(std::string function) {
    args->activateFuncName = function;
    if (function == "sigmoid") {
        args->activateFunc = sigmoid;
    } else if (function == "relu") {
        args->activateFunc = relu;
    } else if (function == "tanh") {
        args->activateFunc = tanh;
    } else {
        std::cout << "Wrong function name!" << std::endl;
    }
}

Lycoris *Lycoris::importLycoris(std::string path, uint32_t capacity) {

}

void Lycoris::exportLycoris(std::string path) {

}
