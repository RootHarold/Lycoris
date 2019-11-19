/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include <algorithm>
#include "utils.h"
#include <iostream>

namespace LycorisNet {

    LycorisUtils::LycorisUtils() {

    }

    LycorisUtils::~LycorisUtils() {

    }

    Individual *LycorisUtils::mutateIndividual(LycorisNet::Individual &in) {
        // Clone the individual.
        auto offspring = in.clone();

        auto ran = LycorisRandomFloat(0, 1);

        if (ran < in.args->p1) { // p1
            // Add the new node between a connection.
            auto nodeOld = (*(offspring->nodeMap))[(*(offspring->nodeSlice))[LycorisRandomUint32_t(
                    uint32_t(offspring->nodeSlice->size()))]];

            // If there is a suitable connection.
            if (!nodeOld->genomeMap->empty()) {
                Gen genOld;
                Ome omeOld;

                auto ranPos = LycorisRandomUint32_t(uint32_t(nodeOld->genomeMap->size()));
                uint32_t count = 0;
                for (auto iter = nodeOld->genomeMap->begin(); iter != nodeOld->genomeMap->end(); ++iter) {
                    if (count == ranPos) {
                        genOld = iter->first;
                        omeOld = iter->second;
                        break;
                    }
                    count++;
                }

                auto n = new Node(offspring->nodeSum, 1);
                n->initializeBias(LycorisRandomFloat(in.args->biasA, in.args->biasB));
                offspring->nodeSum++;
                Gen g1(genOld.in, n->nodeNum);
                Ome o1(1.0, offspring->innovationNum);
                offspring->innovationNum++;
                Gen g2(n->nodeNum, genOld.out);
                Ome o2(omeOld.weight, offspring->innovationNum);
                offspring->innovationNum++;
                n->genomeMap->insert(std::make_pair(g1, o1));
                nodeOld->genomeMap->erase(genOld);
                nodeOld->genomeMap->insert(std::make_pair(g2, o2));
                offspring->nodeMap->insert(std::make_pair(n->nodeNum, n));

                auto iter = std::find(offspring->nodeSlice->begin(), offspring->nodeSlice->end(), genOld.out);
                offspring->nodeSlice->insert(iter, n->nodeNum);
            }
        } else if (ran >= in.args->p1 && ran < in.args->p1 + in.args->p2) { // p2
            // Delete a node.
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index = LycorisRandomUint32_t(length);
            auto sliceIndex = (*(offspring->nodeSlice))[index];
            if ((*(offspring->nodeMap))[sliceIndex]->nodeType == 1) {
                for (uint32_t i = index + 1; i < length; ++i) {
                    auto outputNum = (*(offspring->nodeSlice))[i];
                    auto n = (*(offspring->nodeMap))[outputNum];
                    n->genomeMap->erase(Gen(sliceIndex, outputNum));
                }
                auto temp = (*(offspring->nodeMap))[sliceIndex];
                offspring->nodeMap->erase(sliceIndex);
                offspring->nodeSlice->erase(offspring->nodeSlice->begin() + index);
                delete temp;
            }
        } else if (ran >= in.args->p1 + in.args->p2 && ran < in.args->p1 + in.args->p2 + in.args->p3) { // p3
            // Add a new connection between two nodes.
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index1 = LycorisRandomUint32_t(length);
            auto index2 = index1 + LycorisRandomUint32_t(length - index1);

            if (index1 != index2) {
                auto inputNum = (*(offspring->nodeSlice))[index1];
                auto outputNum = (*(offspring->nodeSlice))[index2];
                auto inputNode = (*(offspring->nodeMap))[inputNum];
                auto outputNode = (*(offspring->nodeMap))[outputNum];

                if (!((inputNode->nodeType == 0 && outputNode->nodeType == 0) || inputNode->nodeType == 2)) {
                    Gen g(inputNum, outputNum);
                    if (outputNode->genomeMap->find(g) == outputNode->genomeMap->end()) {
                        Ome o(LycorisRandomFloat(0, 1) * (in.args->weightB - in.args->weightA) + in.args->weightA,
                              offspring->innovationNum);
                        offspring->innovationNum++;
                        outputNode->genomeMap->insert(std::make_pair(g, o));
                    }
                }
            }
        } else { // p4
            // Delete a connection.
            auto n = (*(offspring->nodeMap))[(*(offspring->nodeSlice))[LycorisRandomUint32_t(
                    uint32_t(offspring->nodeSlice->size()))]];

            if (!(n->genomeMap->empty())) {
                Gen g;
                auto ranPos = LycorisRandomUint32_t(uint32_t(n->genomeMap->size()));
                uint32_t count = 0;
                for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                    if (count == ranPos) {
                        g = iter->first;
                        break;
                    }
                    count++;
                }
                n->genomeMap->erase(g);
            }
        }

        return offspring;
    }

    void LycorisUtils::mutateIndividual_built_in(LycorisNet::Individual &in) {
        auto offspring = &in;

        auto ran = LycorisRandomFloat(0, 1);

        if (ran < in.args->p1) { // p1
            // Add the new node between a connection.
            auto nodeOld = (*(offspring->nodeMap))[(*(offspring->nodeSlice))[LycorisRandomUint32_t(
                    uint32_t(offspring->nodeSlice->size()))]];

            // If there is a suitable connection.
            if (!nodeOld->genomeMap->empty()) {
                Gen genOld;
                Ome omeOld;

                auto ranPos = LycorisRandomUint32_t(uint32_t(nodeOld->genomeMap->size()));
                uint32_t count = 0;
                for (auto iter = nodeOld->genomeMap->begin(); iter != nodeOld->genomeMap->end(); ++iter) {
                    if (count == ranPos) {
                        genOld = iter->first;
                        omeOld = iter->second;
                        break;
                    }
                    count++;
                }

                auto n = new Node(offspring->nodeSum, 1);
                n->initializeBias(LycorisRandomFloat(in.args->biasA, in.args->biasB));
                offspring->nodeSum++;
                Gen g1(genOld.in, n->nodeNum);
                Ome o1(1.0, offspring->innovationNum);
                offspring->innovationNum++;
                Gen g2(n->nodeNum, genOld.out);
                Ome o2(omeOld.weight, offspring->innovationNum);
                offspring->innovationNum++;
                n->genomeMap->insert(std::make_pair(g1, o1));
                nodeOld->genomeMap->erase(genOld);
                nodeOld->genomeMap->insert(std::make_pair(g2, o2));
                offspring->nodeMap->insert(std::make_pair(n->nodeNum, n));

                auto iter = std::find(offspring->nodeSlice->begin(), offspring->nodeSlice->end(), genOld.out);
                offspring->nodeSlice->insert(iter, n->nodeNum);
            }
        } else if (ran >= in.args->p1 && ran < in.args->p1 + in.args->p2) { // p2
            // Delete a node.
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index = LycorisRandomUint32_t(length);
            auto sliceIndex = (*(offspring->nodeSlice))[index];
            if ((*(offspring->nodeMap))[sliceIndex]->nodeType == 1) {
                for (uint32_t i = index + 1; i < length; ++i) {
                    auto outputNum = (*(offspring->nodeSlice))[i];
                    auto n = (*(offspring->nodeMap))[outputNum];
                    n->genomeMap->erase(Gen(sliceIndex, outputNum));
                }
                auto temp = (*(offspring->nodeMap))[sliceIndex];
                offspring->nodeMap->erase(sliceIndex);
                offspring->nodeSlice->erase(offspring->nodeSlice->begin() + index);
                delete temp;
            }
        } else if (ran >= in.args->p1 + in.args->p2 && ran < in.args->p1 + in.args->p2 + in.args->p3) { // p3
            // Add a new connection between two nodes.
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index1 = LycorisRandomUint32_t(length);
            auto index2 = index1 + LycorisRandomUint32_t(length - index1);

            if (index1 != index2) {
                auto inputNum = (*(offspring->nodeSlice))[index1];
                auto outputNum = (*(offspring->nodeSlice))[index2];
                auto inputNode = (*(offspring->nodeMap))[inputNum];
                auto outputNode = (*(offspring->nodeMap))[outputNum];

                if (!((inputNode->nodeType == 0 && outputNode->nodeType == 0) || inputNode->nodeType == 2)) {
                    Gen g(inputNum, outputNum);
                    if (outputNode->genomeMap->find(g) == outputNode->genomeMap->end()) {
                        Ome o(LycorisRandomFloat(0, 1) * (in.args->weightB - in.args->weightA) + in.args->weightA,
                              offspring->innovationNum);
                        offspring->innovationNum++;
                        outputNode->genomeMap->insert(std::make_pair(g, o));
                    }
                }
            }
        } else { // p4
            // Delete a connection.
            auto n = (*(offspring->nodeMap))[(*(offspring->nodeSlice))[LycorisRandomUint32_t(
                    uint32_t(offspring->nodeSlice->size()))]];

            if (!(n->genomeMap->empty())) {
                Gen g;
                auto ranPos = LycorisRandomUint32_t(uint32_t(n->genomeMap->size()));
                uint32_t count = 0;
                for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                    if (count == ranPos) {
                        g = iter->first;
                        break;
                    }
                    count++;
                }
                n->genomeMap->erase(g);
            }
        }
    }

    void LycorisUtils::addHiddenLayer(Individual &in, uint32_t num) {
        auto offspring = &in;
        for (uint32_t i = 0; i < num; ++i) {
            auto n = new Node(offspring->nodeSum, 1);
            n->initializeBias(LycorisRandomFloat(in.args->biasA, in.args->biasB));
            offspring->nodeSum++;

            for (uint32_t j = 0; j < offspring->inputNum; ++j) {
                Gen g(j, n->nodeNum);
                Ome o(LycorisRandomFloat(in.args->weightA, in.args->weightB), offspring->innovationNum);
                offspring->innovationNum++;
                n->genomeMap->insert(std::make_pair(g, o));
            }

            for (uint32_t j = offspring->inputNum; j < offspring->inputNum + offspring->outputNum; ++j) {
                Gen g(n->nodeNum, j);
                Ome o(LycorisRandomFloat(in.args->weightA, in.args->weightB), offspring->innovationNum);
                offspring->innovationNum++;
                (*(offspring->nodeMap))[j]->genomeMap->insert(std::make_pair(g, o));
            }

            offspring->nodeMap->insert(std::make_pair(n->nodeNum, n));
            offspring->nodeSlice->insert(offspring->nodeSlice->begin() + (offspring->inputNum + i), n->nodeNum);
        }
    }

    void LycorisUtils::addHiddenNodes(LycorisNet::Individual &in, uint32_t num) {
        auto offspring = &in;
        for (uint32_t i = 0; i < num; ++i) {
            auto n = new Node(offspring->nodeSum, 1);
            n->initializeBias(LycorisRandomFloat(in.args->biasA, in.args->biasB));
            offspring->nodeSum++;
            offspring->nodeMap->insert(std::make_pair(n->nodeNum, n));
            offspring->nodeSlice->insert(offspring->nodeSlice->begin() + (offspring->inputNum + i), n->nodeNum);
        }
    }

    void LycorisUtils::addConnections(LycorisNet::Individual &in, uint32_t num) {
        auto offspring = &in;
        for (uint32_t i = 0; i < num; ++i) {
            // Add a new connection between two nodes.
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index1 = LycorisRandomUint32_t(length);
            auto index2 = index1 + LycorisRandomUint32_t(length - index1);

            if (index1 != index2) {
                auto inputNum = (*(offspring->nodeSlice))[index1];
                auto outputNum = (*(offspring->nodeSlice))[index2];
                auto inputNode = (*(offspring->nodeMap))[inputNum];
                auto outputNode = (*(offspring->nodeMap))[outputNum];

                if (!((inputNode->nodeType == 0 && outputNode->nodeType == 0) || inputNode->nodeType == 2)) {
                    Gen g(inputNum, outputNum);
                    if (outputNode->genomeMap->find(g) == outputNode->genomeMap->end()) {
                        Ome o(LycorisRandomFloat(0, 1) * (in.args->weightB - in.args->weightA) + in.args->weightA,
                              offspring->innovationNum);
                        offspring->innovationNum++;
                        outputNode->genomeMap->insert(std::make_pair(g, o));
                    }
                }
            }
        }
    }

    SortFitness::SortFitness() {

    }

    SortFitness::SortFitness(float fitness, uint32_t individualNum) {
        this->fitness = fitness;
        this->individualNum = individualNum;
    }

    SortFitness::~SortFitness() {

    }

}
