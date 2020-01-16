/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
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

    // Mutating the individuals.
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
                for (auto &iter : *nodeOld->genomeMap) {
                    if (count == ranPos) {
                        genOld = iter.first;
                        omeOld = iter.second;
                        break;
                    }
                    count++;
                }

                auto n = new Node(offspring->nodeSum, 1);
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
                        Ome o(LycorisRandomFloat(in.args->weightA, in.args->weightB), offspring->innovationNum);
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
                for (auto &iter : *n->genomeMap) {
                    if (count == ranPos) {
                        g = iter.first;
                        break;
                    }
                    count++;
                }
                n->genomeMap->erase(g);
            }
        }

        return offspring;
    }

    // Adding hidden nodes of neural network in preheating process.
    void LycorisUtils::addHiddenNodes(LycorisNet::Individual &in, uint32_t num) {
        for (uint32_t i = 0; i < num; ++i) {
            auto n = new Node(in.nodeSum, 1);
            in.nodeSum++;
            in.nodeMap->insert(std::make_pair(n->nodeNum, n));
            in.nodeSlice->insert(in.nodeSlice->begin() + (in.inputNum + i), n->nodeNum);
        }
    }

    // Adding the connections of neural network in preheating process.
    void LycorisUtils::addConnections(LycorisNet::Individual &in, uint32_t num) {
        auto length = uint32_t(in.nodeSlice->size());
        std::vector<std::vector<uint32_t> > arr;
        uint32_t pointer = 0;

        std::vector<uint32_t> input_arr(in.inputNum);
        in.layers.push_back(in.inputNum);
        for (uint32_t i = 0; i < in.inputNum; ++i) {
            input_arr[i] = (*(in.nodeSlice))[pointer];
            ++pointer;
        }
        arr.push_back(input_arr);

        auto left = length - in.inputNum - in.outputNum - in.args->depth + 2;
        std::vector<uint32_t> split_arr;
        split_arr.push_back(0);
        split_arr.push_back(left);
        for (uint32_t i = 0; i < in.args->depth - 3; ++i) {
            split_arr.push_back(LycorisRandomUint32_t(left));
        }
        std::sort(split_arr.begin(), split_arr.end());

        for (uint32_t i = 1; i < in.args->depth - 1; ++i) {
            auto temp_length = split_arr[i] - split_arr[i - 1] + 1;
            std::vector<uint32_t> temp_arr(temp_length);
            in.layers.push_back(temp_length);

            for (uint32_t j = 0; j < temp_length; ++j) {
                temp_arr[j] = (*(in.nodeSlice))[pointer];
                ++pointer;
            }

            arr.push_back(temp_arr);
        }

        std::vector<uint32_t> output_arr(in.outputNum);
        in.layers.push_back(in.outputNum);
        for (uint32_t i = 0; i < in.outputNum; ++i) {
            output_arr[i] = (*(in.nodeSlice))[pointer];
            ++pointer;
        }
        arr.push_back(output_arr);

        for (uint32_t i = 0; i < num; ++i) {
            // Add a new connection between two nodes.
            auto index1 = LycorisRandomUint32_t(in.args->depth);
            auto index2 = index1 + LycorisRandomUint32_t(in.args->depth - index1);

            if (index1 != index2) {
                auto inputNum = arr[index1][LycorisRandomUint32_t(arr[index1].size())];
                auto outputNum = arr[index2][LycorisRandomUint32_t(arr[index2].size())];

                auto outputNode = (*(in.nodeMap))[outputNum];

                Gen g(inputNum, outputNum);
                if (outputNode->genomeMap->find(g) == outputNode->genomeMap->end()) {
                    Ome o(LycorisRandomFloat(in.args->weightA, in.args->weightB), in.innovationNum);
                    in.innovationNum++;
                    outputNode->genomeMap->insert(std::make_pair(g, o));
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
