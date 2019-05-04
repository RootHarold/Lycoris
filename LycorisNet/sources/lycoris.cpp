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

namespace LycorisNet {

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

        this->forwardFunc = nullHandle;
    }

    Lycoris::~Lycoris() {
        delete args;

        for (auto iter = individualList->begin(); iter != individualList->end(); ++iter) {
            delete *iter;
        }

        delete individualList;
    }

    void Lycoris::setForwardFunc(void (*forwardFunc)(Individual &)) {
        this->forwardFunc = forwardFunc;
    }

    void Lycoris::mutate() {
        auto start = new uint32_t[args->cpuNum];
        auto end = new uint32_t[args->cpuNum];
        oldLength = uint32_t(individualList->size());
        auto mutateLength = uint32_t(float(oldLength) * args->mutateOdds);
        tempList.resize(mutateLength);
        auto part = mutateLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t i = 0; i < temp; ++i) {
            start[i] = i * part;
            end[i] = (i + 1) * part;
        }
        start[temp] = temp * part;
        end[temp] = mutateLength;

        std::vector<std::thread> threads;
        for (uint32_t i = 0; i < args->cpuNum; ++i) {
            threads.emplace_back(std::thread(&Lycoris::mutateCore, this, start[i], end[i]));
        }
        for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
            (*iter).join();
        }

        for (uint32_t i = 0; i < mutateLength; ++i) {
            individualList->push_back(tempList[i]);
        }

        delete[] start;
        delete[] end;
    }

    void Lycoris::mutateCore(uint32_t start, uint32_t end) {
        for (uint32_t i = start; i < end; ++i) {
            tempList[i] = args->utils->mutateIndividual(
                    *((*individualList)[args->utils->LycorisRandomUint32_t(oldLength)]));
        }
    }

    void Lycoris::forward() {
        auto start = new uint32_t[args->cpuNum];
        auto end = new uint32_t[args->cpuNum];
        auto individualLength = uint32_t(individualList->size());
        auto part = individualLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t i = 0; i < temp; ++i) {
            start[i] = i * part;
            end[i] = (i + 1) * part;
        }
        start[temp] = temp * part;
        end[temp] = individualLength;

        std::vector<std::thread> threads;
        for (uint32_t i = 0; i < args->cpuNum; ++i) {
            threads.emplace_back(std::thread(&Lycoris::forwardCore, this, start[i], end[i]));
        }
        for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
            (*iter).join();
        }

        delete[] start;
        delete[] end;
    }

    void Lycoris::forwardCore(uint32_t start, uint32_t end) {
        for (uint32_t i = start; i < end; ++i) {
            forwardFunc(*((*individualList)[i]));
        }
    }

    void Lycoris::chooseElite() {
        uint32_t totalLength = individualList->size();
        if (totalLength == 0) {
            std::cout << "All died." << std::endl;
            exit(1);
        }

        std::vector<SortFitness> sortList(totalLength);
        for (uint32_t i = 0; i < totalLength; ++i) {
            // To fix the NaN & Inf problems.
            if (std::isnan((*individualList)[i]->fitness) || std::isinf((*individualList)[i]->fitness)) {
                (*individualList)[i]->fitness = -FLT_MAX;
            }

            sortList[i] = SortFitness((*individualList)[i]->fitness, i);
        }
        std::sort(sortList.begin(), sortList.end(), LycorisUtils::compareFitness);
        auto last = sortList[totalLength - 1];
        best = (*individualList)[last.individualNum];

        auto newIndividualList = new std::vector<Individual *>();
        auto newLength = uint32_t(float(capacity) / (1 + args->mutateOdds));
        if (newLength == 0) {
            newLength = 1;
        }

        uint32_t memSum = 0;
        auto z = totalLength;
        for (; z > totalLength - newLength; --z) {
            if (z == 0) { // This needs elegant repairs.
                break;
            }
            auto temp = sortList[z - 1];
            auto tempIndividual = (*individualList)[temp.individualNum];
            if (args->memLimitFlag) {
                memSum += tempIndividual->getSize();
            }
            newIndividualList->push_back(tempIndividual);
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

        if (totalLength - newLength > 0) {
            for (uint32_t i = 0; i < totalLength - newLength; ++i) {
                if (i == totalLength) {
                    break;
                }
                auto temp = sortList[i];
                auto tempIndividual = (*individualList)[temp.individualNum];
                delete tempIndividual;
            }
        }
        delete individualList;
        individualList = newIndividualList;
    }

    void Lycoris::runLycoris() {
        if (args->firstRun) {
            args->firstRun = false;

            auto initialCapacity = uint32_t(float(capacity) / (1 + args->mutateOdds));
            if (initialCapacity == 0) {
                initialCapacity = 1;
            }

            individualList = new std::vector<Individual *>(initialCapacity);
            for (uint32_t i = 0; i < initialCapacity; ++i) {
                (*individualList)[i] = new Individual(inputNum, outputNum, args);
            }
            best = (*individualList)[0];
        }

        // Mutating.
        mutate();
        // Forward calculation.
        forward();
        // Sorting and choosing some individuals with higher fitness.
        chooseElite();

        // Memory exceeds the limit.
        if (args->memOverFlag) {
            args->p1 = 0;
            args->p3 = 0;
            auto temp = args->p2 + args->p4 + args->p5 + args->p6;
            args->p2 /= temp;
            args->p4 /= temp;
            args->p5 /= temp;
            args->p6 /= temp;
        } else {
            args->p1 = args->p1B;
            args->p2 = args->p2B;
            args->p3 = args->p3B;
            args->p4 = args->p4B;
            args->p5 = args->p5B;
            args->p6 = args->p6B;
        }
    }

    void Lycoris::runLycoris(uint32_t n) {
        for (uint32_t i = 0; i < n; ++i) {
            runLycoris();
        }
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

        auto data = LycorisUtils::split(str);
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
        radiata->args->firstRun = false;
        source->args = radiata->args;
        radiata->setActivateFunc(activateFuncName);

        auto initialCapacity = uint32_t(float(capacity) / (1 + radiata->args->mutateOdds));
        if (initialCapacity == 0) {
            initialCapacity = 1;
        }
        radiata->individualList = new std::vector<Individual *>(initialCapacity);
        for (uint32_t i = 0; i < initialCapacity; ++i) {
            (*radiata->individualList)[i] = source->clone();
        }
        radiata->best = (*radiata->individualList)[0];

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


    void Lycoris::setMutateArgs(float *p) {
        args->p1 = p[0];
        args->p2 = p[1];
        args->p3 = p[2];
        args->p4 = p[3];
        args->p5 = p[4];
        args->p6 = p[5];
        args->p1B = p[0];
        args->p2B = p[1];
        args->p3B = p[2];
        args->p4B = p[3];
        args->p5B = p[4];
        args->p6B = p[5];
    }

    void Lycoris::setCpuCores(uint32_t num) {
        args->cpuNum = num;
    }

    void Lycoris::setMutateOdds(float odds) {
        args->mutateOdds = odds;
    }

}
