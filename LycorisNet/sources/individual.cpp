/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include "individual.h"
#include "utils.h"

namespace LycorisNet {

    Individual::Individual() {

    }

    Individual::Individual(uint32_t inputNum, uint32_t outputNum, Args *args) {
        this->inputNum = inputNum;
        this->outputNum = outputNum;
        this->args = args;
        nodeSum = 0;
        innovationNum = 0;
        initialize();
    }

    Individual::~Individual() {
        delete nodeSlice;

        for (auto iter = nodeMap->begin(); iter != nodeMap->end(); ++iter) {
            delete iter->second;
        }
        delete nodeMap;
    }

    void Individual::initialize() {
        nodeMap = new std::map<uint32_t, Node *>();
        nodeSlice = new std::vector<uint32_t>(inputNum + outputNum);

        for (uint32_t i = 0; i < inputNum; ++i) {
            auto temp = nodeSum;
            auto newNode = new Node(temp, 0);
            newNode->initializeBias(args->utils->LycorisRandomFloat(args->biasA, args->biasB));
            nodeMap->insert(std::make_pair(temp, newNode));
            (*nodeSlice)[temp] = temp;
            nodeSum++;
        }

        for (uint32_t i = 0; i < outputNum; ++i) {
            auto temp = nodeSum;
            auto newNode = new Node(temp, 2);
            newNode->initializeBias(args->utils->LycorisRandomFloat(args->biasA, args->biasB));
            nodeMap->insert(std::make_pair(temp, newNode));
            (*nodeSlice)[temp] = temp;
            nodeSum++;
        }
    }

    void Individual::setInput(float *input) {
        for (uint32_t i = 0; i < inputNum; ++i) {
            (*nodeMap)[i]->value = input[i];
        }
    }

    void Individual::forward(float *input) {
        setInput(input);
        forward();
    }

    void Individual::forward(float *input, float *output) {
        setInput(input);
        forward();
        getOutput(output);
    }

    void Individual::getOutput(float *output) {
        uint32_t pointer = 0;
        for (uint32_t i = inputNum; i < nodeSlice->size(); ++i) {
            auto temp = (*nodeMap)[(*nodeSlice)[i]];
            if (temp->nodeType == 2) {
                output[pointer] = temp->value;
                pointer++;
            }
        }
    }

    void Individual::forward() {
        std::map<uint32_t, bool> clean;

        for (uint32_t i = inputNum; i < nodeSlice->size(); ++i) {
            auto index = (*nodeSlice)[i];
            auto n = (*nodeMap)[index];
            float f = 0;

            if (n->genomeMap->empty()) {
                if (n->nodeType == 1) {
                    clean.insert(std::make_pair(index, true));
                }
            } else {
                for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                    auto g = iter->first;
                    auto o = iter->second;
                    if (o.isEnable) {
                        auto temp = nodeMap->find(g.in);
                        if (temp != nodeMap->end()) {
                            f += temp->second->value * o.weight;
                        }
                    }
                }
                n->value = args->activateFunc(f + n->bias);
            }
        }

        // Clean empty nodes according to a certain chance (cleanOdds).
        if (args->utils->LycorisRandomFloat(0, 1) < args->cleanOdds && !clean.empty()) {
            for (auto iter = clean.begin(); iter != clean.end(); ++iter) {
                auto key = iter->first;
                auto temp = (*nodeMap)[key];
                nodeMap->erase(key);
                delete temp;
            }

            for (auto iter = nodeSlice->begin(); iter != nodeSlice->end();) {
                if (clean.find(*iter) != clean.end()) {
                    iter = nodeSlice->erase(iter);
                } else {
                    ++iter;
                }
            }
        }
    }

    Individual *Individual::clone() {
        auto dulplicate = new Individual();
        dulplicate->inputNum = inputNum;
        dulplicate->outputNum = outputNum;
        dulplicate->args = args;
        dulplicate->innovationNum = innovationNum;
        dulplicate->nodeSum = nodeSum;
        dulplicate->fitness = fitness;

        dulplicate->nodeMap = new std::map<uint32_t, Node *>();
        for (auto iter = nodeMap->begin(); iter != nodeMap->end(); ++iter) {
            dulplicate->nodeMap->insert(std::make_pair(iter->first, iter->second->clone()));
        }

        dulplicate->nodeSlice = new std::vector<uint32_t>();
        dulplicate->nodeSlice->assign(nodeSlice->begin(), nodeSlice->end());

        return dulplicate;
    }

    uint32_t Individual::getSize() {
        uint32_t size = 0;
        size += nodeSlice->size();
        for (auto iter = nodeMap->begin(); iter != nodeMap->end(); ++iter) {
            size += iter->second->genomeMap->size();
        }
        return size;
    }

}
