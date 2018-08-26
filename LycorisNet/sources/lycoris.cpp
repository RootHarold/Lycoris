/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include "lycoris.h"
#include <algorithm>
#include <cfloat>
#include <iostream>
#include <fstream>

// The null handle of 'forwardFunc'.
void nullHandle(Individual &in) {
    std::cout << "Please set 'forwardFunc'." << std::endl;
    exit(1);
}

Lycoris::Lycoris(uint32_t capacity, uint32_t inputNum, uint32_t outputNum) {
    this->capacity = capacity;
    this->inputNum = inputNum;
    this->outputNum = outputNum;
    args = new Args();

    this->speciesList = nullptr;
    this->forwardFunc = nullHandle;
}

Lycoris::~Lycoris() {
    delete args;

    if (speciesList != nullptr) {
        for (auto iter = speciesList->begin(); iter != speciesList->end(); ++iter) {
            delete *iter;
        }

        delete speciesList;
    }
}

void Lycoris::setForwardFunc(void (*forwardFunc)(Individual &)) {
    this->forwardFunc = forwardFunc;
}

void Lycoris::setMaxTock(uint32_t num) {
    args->maxEmergeTock = num;
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
            tempList1[i][j] = mateIndividual(*((*l)[LycorisUtils::LycorisRandomUint32_t(oldLength[i])]),
                                             *((*l)[LycorisUtils::LycorisRandomUint32_t(oldLength[i])]));
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
            tempList1[i][j] = mutateIndividual(*((*l)[LycorisUtils::LycorisRandomUint32_t(oldLength[i])]));
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
                auto dis = distance(*temp, *((*l)[LycorisUtils::LycorisRandomUint32_t(oldLength[k])]));
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
    if (args->memOverFlag) { // Memory exceeds the limit.
        auto mutateTimeE = LycorisUtils::LycorisRandomUint32_t(args->maxMutateTime) + 1;
        auto mateOddsE = LycorisUtils::LycorisRandomFloat(1, 2);
        auto mutateOddsE = LycorisUtils::LycorisRandomFloat(1, 2);
        float remain = 1;
        float p1E = 0;
        remain -= p1E;
        auto p2E = LycorisUtils::LycorisRandomFloat(0, 1) * remain;
        remain -= p2E;
        float p3E = 0;
        remain -= p3E;
        auto p4E = LycorisUtils::LycorisRandomFloat(0, 1) * remain;
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
        auto mutateTimeE = LycorisUtils::LycorisRandomUint32_t(args->maxMutateTime) + 1;
        auto mateOddsE = LycorisUtils::LycorisRandomFloat(1, 2);
        auto mutateOddsE = LycorisUtils::LycorisRandomFloat(1, 2);
        float remain = 1;
        auto p1E = LycorisUtils::LycorisRandomFloat(0, 1);
        remain -= p1E;
        auto p2E = LycorisUtils::LycorisRandomFloat(0, 1) * remain;
        remain -= p2E;
        auto p3E = LycorisUtils::LycorisRandomFloat(0, 1) * remain;
        remain -= p3E;
        auto p4E = LycorisUtils::LycorisRandomFloat(0, 1) * remain;
        remain -= p4E;
        auto p5E = LycorisUtils::LycorisRandomFloat(0, 1) * remain;
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
        if (!args->gapListFlag) {
            auto temp1 = LycorisUtils::slope(*args->gapList);
            std::vector<float> tempList(args->slopeTick);
            tempList.assign(args->gapList->end() - args->slopeTick, args->gapList->end());
            auto temp2 = LycorisUtils::slope(tempList);

            if (temp1 >= temp2) {
                args->miss++;
                args->hit = 0;
                if (args->miss == 2) {
                    if (args->emergeTock > 1) {
                        args->emergeTock /= 2;
                    }

                    if (args->slopeTick < args->maxSlopeTick) {
                        args->slopeTick *= 2;
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
                    if (args->emergeTock < args->maxEmergeTock) {
                        args->emergeTock *= 2;
                    }

                    if (args->slopeTick > 2) {
                        args->slopeTick /= 2;
                    }

                    args->hit = 1;
                }
            }
        }
        args->checkFlag = false;
    }

    if (args->emergeTick == args->emergeTock) {
        emergeArgs();
        args->checkFlag = true;
        args->emergeTick = 0;
    } else {
        args->emergeTick += 1;
    }
}

void Lycoris::chooseElite() {
    uint32_t totalLength = 0;
    for (auto iter = speciesList->begin(); iter != speciesList->end(); ++iter) {
        totalLength += (*iter)->individualList->size();
    }

    if (totalLength == 0) {
        std::cout << "All died." << std::endl;
        exit(1);
    }

    std::vector<SortFitness> sortList(totalLength);
    uint32_t pointer = 0;
    for (uint32_t i = 0; i < speciesList->size(); ++i) {
        auto temp = (*speciesList)[i]->individualList;
        for (uint32_t j = 0; j < temp->size(); ++j) {
            // To fix the NaN & Inf problems.
            if (std::isnan((*temp)[j]->fitness) || std::isinf((*temp)[j]->fitness)) {
                (*temp)[j]->fitness = -FLT_MAX;
            }

            sortList[pointer] = SortFitness((*temp)[j]->fitness, i, j);
            pointer++;
        }
    }
    std::sort(sortList.begin(), sortList.end(), compareFitness);
    auto last = sortList[totalLength - 1];
    best = (*(*speciesList)[last.specieNum]->individualList)[last.individualNum];

    auto newSpecieList = new std::vector<Species *>(specieLength);
    for (uint32_t i = 0; i < specieLength; ++i) {
        (*newSpecieList)[i] = new Species();
    }
    auto newLength = uint32_t(float(capacity) / ((1 + args->mateOdds) * (1 + args->mutateOdds)));
    uint32_t memSum = 0;
    auto z = totalLength;
    for (; z > totalLength - newLength; --z) {
        if (z == 0) { // This needs elegant repairs.
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
        if (memSum > (totalLength - z) * args->limitSize) { // Memory exceeds the limit.
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

    if (args->gapListFlag) { // len < gapLength
        auto length = args->gapList->size();
        if (length == args->maxGap) {
            args->gapListFlag = false;
        }
        args->gapList->push_back(best->fitness);
    } else {
        args->gapList->erase(args->gapList->begin());
        args->gapList->push_back(best->fitness);
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
        for (uint32_t i = 0; i < initialCapacity; ++i) {
            (*(specie->individualList))[i] = new Individual(inputNum, outputNum, args);
        }
        speciesList = new std::vector<Species *>();
        speciesList->push_back(specie);
        best = (*(*speciesList)[0]->individualList)[0];
    }

    // Mating.
    mate();
    // Computing distances of new individuals.
    classify();
    // Mutating.
    mutate();
    // Computing distances of new individuals.
    classify();
    // Forward calculation.
    forward();
    // Changing some parameters automatically.
    autoParameter();
    // Sorting and choosing some individuals with higher fitness.
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
        args->activateFunc = LycorisUtils::sigmoid;
    } else if (function == "relu") {
        args->activateFunc = LycorisUtils::relu;
    } else if (function == "tanh") {
        args->activateFunc = LycorisUtils::tanh;
    } else {
        std::cout << "Wrong function name!" << std::endl;
        exit(1);
    }
}

Lycoris *Lycoris::importLycoris(std::string path, uint32_t capacity) {
    std::ifstream infile(path);
    std::string str;
    std::getline(infile, str);
    infile.close();

    if (str.empty()) {
        std::cout << "Wrong path." << std::endl;
        exit(1);
    }

    auto data = split(str);
    auto source = new Individual();
    source->inputNum = uint32_t(std::stoul(data[0]));
    source->outputNum = uint32_t(std::stoul(data[1]));
    source->innovationNum = uint32_t(std::stoul(data[2]));
    source->nodeSum = uint32_t(std::stoul(data[3]));
    source->fitness = std::stof(data[4]);

    source->nodeSlice = new std::vector<uint32_t>(std::stoul(data[5]));
    uint32_t pointer = 6;
    for (uint32_t i = 0; i < std::stoul(data[5]); ++i) {
        (*(source->nodeSlice))[i] = uint32_t(std::stoul(data[pointer]));
        pointer++;
    }

    auto mapLength = uint32_t(std::stoul(data[pointer]));
    pointer++;
    source->nodeMap = new std::map<uint32_t, Node *>();
    for (uint32_t i = 0; i < mapLength; ++i) {
        auto key = uint32_t(std::stoul(data[pointer]));
        pointer++;
        auto temp1 = data[pointer];
        pointer++;
        auto temp2 = data[pointer];
        pointer++;
        auto n = new Node(uint32_t(std::stoul(temp1)), uint32_t(std::stoul(temp2)));
        n->value = std::stof(data[pointer]);
        pointer++;
        n->bias = std::stof(data[pointer]);
        pointer++;

        auto genomeLength = uint32_t(std::stoul(data[pointer]));
        pointer++;
        for (uint32_t j = 0; j < genomeLength; ++j) {
            Gen g;
            g.in = uint32_t(std::stoul(data[pointer]));
            pointer++;
            g.out = uint32_t(std::stoul(data[pointer]));
            pointer++;
            Ome o;
            o.weight = std::stof(data[pointer]);
            pointer++;
            o.isEnable = (std::stoul(data[pointer]) == 1);
            pointer++;
            o.innovationNum = uint32_t(std::stoul(data[pointer]));
            pointer++;
            (*(n->genomeMap))[g] = o;
        }

        (*(source->nodeMap))[key] = n;
    }

    auto activateFuncName = data[pointer];

    auto radiata = new Lycoris(capacity, source->inputNum, source->outputNum);
    source->args = radiata->args;
    radiata->setActivateFunc(activateFuncName);

    auto specie = new Species();
    auto initialCapacity = uint32_t(
            float(capacity) / ((1 + radiata->args->mateOdds) * (1 + radiata->args->mutateOdds)));
    specie->individualList->resize(initialCapacity);
    for (uint32_t i = 0; i < initialCapacity; ++i) {
        (*(specie->individualList))[i] = source->clone();
    }
    radiata->speciesList = new std::vector<Species *>();
    radiata->speciesList->push_back(specie);
    radiata->best = (*(*(radiata->speciesList))[0]->individualList)[0];
    radiata->args->firstRun = false;

    delete source;
    return radiata;
}

void Lycoris::exportLycoris(std::string path) {
    std::ofstream outfile;
    outfile.open(path);

    outfile << std::to_string(best->inputNum) << " ";
    outfile << std::to_string(best->outputNum) << " ";
    outfile << std::to_string(best->innovationNum) << " ";
    outfile << std::to_string(best->nodeSum) << " ";
    outfile << std::to_string(best->fitness) << " ";

    outfile << std::to_string(best->nodeSlice->size()) << " ";
    for (auto iter = best->nodeSlice->begin(); iter != best->nodeSlice->end(); ++iter) {
        outfile << std::to_string(*iter) << " ";
    }

    outfile << std::to_string(best->nodeMap->size()) << " ";
    for (auto iter = best->nodeMap->begin(); iter != best->nodeMap->end(); ++iter) {
        outfile << std::to_string(iter->first) << " ";
        outfile << std::to_string(iter->second->nodeNum) << " ";
        outfile << std::to_string(iter->second->nodeType) << " ";
        outfile << std::to_string(iter->second->value) << " ";
        outfile << std::to_string(iter->second->bias) << " ";

        outfile << std::to_string(iter->second->genomeMap->size()) << " ";
        for (auto it = iter->second->genomeMap->begin(); it != iter->second->genomeMap->end(); ++it) {
            outfile << std::to_string(it->first.in) << " ";
            outfile << std::to_string(it->first.out) << " ";
            outfile << std::to_string(it->second.weight) << " ";
            if (it->second.isEnable) {
                outfile << "1 ";
            } else {
                outfile << "0 ";
            }
            outfile << std::to_string(it->second.innovationNum) << " ";
        }
    }

    outfile << args->activateFuncName;

    outfile.close();
}
