/*
    Copyright (c) 2019, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include "individual.h"
#include "utils.h"
#include <iostream>

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

    void Individual::forward(float *input, float *output) {
        for (uint32_t i = 0; i < inputNum; ++i) {
            (*nodeMap)[i]->value = input[i];
        }

        for (uint32_t i = inputNum; i < nodeSlice->size(); ++i) {
            auto index = (*nodeSlice)[i];
            auto n = (*nodeMap)[index];
            float f = 0;

            for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                auto g = iter->first;
                auto o = iter->second;
                auto temp = nodeMap->find(g.in);
                if (temp != nodeMap->end()) {
                    f += temp->second->value * o.weight;
                }
            }
            n->value = args->activateFunc(f + n->bias);
        }

        uint32_t pointer = 0;
        for (uint32_t i = inputNum; i < nodeSlice->size(); ++i) {
            auto temp = (*nodeMap)[(*nodeSlice)[i]];
            if (temp->nodeType == 2) {
                output[pointer] = temp->value;
                pointer++;
            }
        }
    }

    void Individual::BP_P() {
        auto individualSize = getSize() + 1;
        if (args->batchFlag) {
            args->batchData = new float *[args->batchSize];

            for (uint32_t i = 0; i < args->batchSize; ++i) {
                args->batchData[i] = new float[individualSize];
            }

            args->midData = new float[individualSize];

            args->batchFlag = false;
        }

        auto start = new uint32_t[args->cpuNum];
        auto end = new uint32_t[args->cpuNum];
        auto part = args->batchSize / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t i = 0; i < temp; ++i) {
            start[i] = i * part;
            end[i] = (i + 1) * part;
        }
        start[temp] = temp * part;
        end[temp] = args->batchSize;

        std::vector<std::thread> threads;
        for (uint32_t i = 0; i < args->cpuNum; ++i) {
            threads.emplace_back(std::thread(&Individual::BP_P_Core, this, start[i], end[i]));
        }
        for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
            (*iter).join();
        }

        part = (individualSize) / args->cpuNum;
        for (uint32_t i = 0; i < temp; ++i) {
            start[i] = i * part;
            end[i] = (i + 1) * part;
        }
        start[temp] = temp * part;
        end[temp] = individualSize;

        std::vector<std::thread> threads2;
        for (uint32_t i = 0; i < args->cpuNum; ++i) {
            threads2.emplace_back(std::thread(&Individual::BP_Compute, this, start[i], end[i], args->midData));
        }
        for (auto iter = threads2.begin(); iter != threads2.end(); ++iter) {
            (*iter).join();
        }

        uint32_t data_p = 0;

        for (uint32_t j = 0; j < nodeSlice->size(); ++j) {
            auto index = (*nodeSlice)[nodeSlice->size() - 1 - j];
            auto n = (*nodeMap)[index];

            for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                (*(n->genomeMap))[iter->first].delta =
                        (*(n->genomeMap))[iter->first].delta * 0.9f + args->midData[data_p++];
                (*(n->genomeMap))[iter->first].weight += (*(n->genomeMap))[iter->first].delta;
            }

            n->delta = n->delta * 0.9f + args->midData[data_p++];
            n->bias += n->delta;
        }

        this->fitness = args->midData[data_p];

        if (std::isnan(this->fitness) || std::isinf(this->fitness)) {
            this->fitness = -FLT_MAX;
        }

        delete[] start;
        delete[] end;
    }

    void Individual::BP_Compute(uint32_t start, uint32_t end, float *midData) {
        for (uint32_t i = start; i < end; ++i) {
            float temp_data = 0.0f;

            for (uint32_t j = 0; j < args->batchSize; ++j) {
                temp_data += args->batchData[j][i];
            }

            temp_data /= args->batchSize;
            midData[i] = temp_data;
        }
    }

    void Individual::BP_P_Core(uint32_t start, uint32_t end) {
        auto ci = this->clone();

        float output[outputNum];
        std::map<uint32_t, float> gradient; // Store the gradient of all nodes.
        for (uint32_t z = start; z < end; ++z) {
            uint32_t data_p = 0;
            ci->forward(ci->args->inputArray[z], output);

            gradient.clear();
            if (ci->args->mode == "predict") { // Predict.
                for (uint32_t j = 0; j < ci->nodeSlice->size(); ++j) {
                    auto index = (*ci->nodeSlice)[ci->nodeSlice->size() - 1 - j];
                    auto n = (*ci->nodeMap)[index];

                    // The output nodes.
                    if (index >= inputNum && index < (inputNum + outputNum)) {
                        gradient[index] = n->value - ci->args->desireArray[z][index - inputNum];
                    }

                    for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                        auto p = gradient.find(iter->first.in);
                        if (p != gradient.end()) {
                            gradient[iter->first.in] +=
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        } else {
                            gradient[iter->first.in] =
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        }

                        args->batchData[z][data_p++] = 0 -
                                                       ci->args->lr * gradient[index] * (n->value > 0 ? 1.0f : 0.2f) *
                                                       (*ci->nodeMap)[iter->first.in]->value;
                    }

                    args->batchData[z][data_p++] = 0 - ci->args->lr * gradient[index] * (n->value > 0 ? 1.0f : 0.2f);
                }
            } else { // Classify.
                LycorisUtils::softmax(output, outputNum);

                for (uint32_t j = 0; j < ci->nodeSlice->size(); ++j) {
                    auto index = (*ci->nodeSlice)[ci->nodeSlice->size() - 1 - j];
                    auto n = (*ci->nodeMap)[index];

                    // The output nodes.
                    if (index >= inputNum && index < (inputNum + outputNum)) {
                        gradient[index] = output[index - inputNum] - ci->args->desireArray[z][index - inputNum];
                    }

                    for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                        auto p = gradient.find(iter->first.in);
                        if (p != gradient.end()) {
                            gradient[iter->first.in] +=
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        } else {
                            gradient[iter->first.in] =
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        }

                        args->batchData[z][data_p++] =
                                0 - ci->args->lr * gradient[index] * (n->value > 0 ? 1.0f : 0.2f) *
                                    (*ci->nodeMap)[iter->first.in]->value;
                    }

                    args->batchData[z][data_p++] = 0 - ci->args->lr * gradient[index] * (n->value > 0 ? 1.0f : 0.2f);
                }
            }

            if (args->mode == "classify") {
                args->batchData[z][data_p] = LycorisUtils::cross_entropy(ci->args->desireArray[z], output, outputNum);
            } else {
                args->batchData[z][data_p] =
                        0 - LycorisUtils::euclidean_distance(output, ci->args->desireArray[z], outputNum);
            }
        }

        delete ci;
    }

    void Individual::BP() {
        float output[outputNum];
        std::map<uint32_t, float> gradient; // Store the gradient of all nodes.
        for (uint32_t z = 0; z < args->batchSize; ++z) {
            this->forward(args->inputArray[z], output);

            gradient.clear();
            if (args->mode == "predict") { // Predict.
                for (uint32_t j = 0; j < nodeSlice->size(); ++j) {
                    auto index = (*nodeSlice)[nodeSlice->size() - 1 - j];
                    auto n = (*nodeMap)[index];

                    // The output nodes.
                    if (index >= inputNum && index < (inputNum + outputNum)) {
                        gradient[index] = n->value - args->desireArray[z][index - inputNum];
                    }

                    for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                        auto p = gradient.find(iter->first.in);
                        if (p != gradient.end()) {
                            gradient[iter->first.in] +=
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        } else {
                            gradient[iter->first.in] =
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        }

                        (*(n->genomeMap))[iter->first].delta_backup = ((*(n->genomeMap))[iter->first].delta_backup * z -
                                                                     args->lr * gradient[index] *
                                                                     (n->value > 0 ? 1.0f : 0.2f) *
                                                                     (*nodeMap)[iter->first.in]->value) / float(z + 1);

                        if (z == args->batchSize - 1) {
                            (*(n->genomeMap))[iter->first].delta = (*(n->genomeMap))[iter->first].delta * 0.9f +
                                                                   (*(n->genomeMap))[iter->first].delta_backup;
                            (*(n->genomeMap))[iter->first].weight += (*(n->genomeMap))[iter->first].delta;
                        }
                    }

                    n->delta_backup = (n->delta_backup * z - args->lr * gradient[index] * (n->value > 0 ? 1.0f : 0.2f)) /
                                    float(z + 1);
                    if (z == args->batchSize - 1) {
                        n->delta = n->delta * 0.9f + n->delta_backup;
                        n->bias += n->delta;
                    }
                }
            } else { // Classify.
                LycorisUtils::softmax(output, outputNum);

                for (uint32_t j = 0; j < nodeSlice->size(); ++j) {
                    auto index = (*nodeSlice)[nodeSlice->size() - 1 - j];
                    auto n = (*nodeMap)[index];

                    // The output nodes.
                    if (index >= inputNum && index < (inputNum + outputNum)) {
                        gradient[index] = output[index - inputNum] - args->desireArray[z][index - inputNum];
                    }

                    for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                        auto p = gradient.find(iter->first.in);
                        if (p != gradient.end()) {
                            gradient[iter->first.in] +=
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        } else {
                            gradient[iter->first.in] =
                                    gradient[index] * iter->second.weight * (n->value > 0 ? 1.0f : 0.2f);
                        }

                        (*(n->genomeMap))[iter->first].delta_backup = ((*(n->genomeMap))[iter->first].delta_backup * z -
                                                                     args->lr * gradient[index] *
                                                                     (n->value > 0 ? 1.0f : 0.2f) *
                                                                     (*nodeMap)[iter->first.in]->value) / float(z + 1);

                        if (z == args->batchSize - 1) {
                            (*(n->genomeMap))[iter->first].delta = (*(n->genomeMap))[iter->first].delta * 0.9f +
                                                                   (*(n->genomeMap))[iter->first].delta_backup;
                            (*(n->genomeMap))[iter->first].weight += (*(n->genomeMap))[iter->first].delta;
                        }
                    }

                    n->delta_backup = (n->delta_backup * z - args->lr * gradient[index] * (n->value > 0 ? 1.0f : 0.2f)) /
                                    float(z + 1);
                    if (z == args->batchSize - 1) {
                        n->delta = n->delta * 0.9f + n->delta_backup;
                        n->bias += n->delta;
                    }
                }
            }

            if (args->mode == "classify") {
                this->fitness =
                        (this->fitness * z + LycorisUtils::cross_entropy(args->desireArray[z], output, outputNum)) /
                        float(z + 1);
            } else {
                this->fitness = (this->fitness * z -
                                 LycorisUtils::euclidean_distance(output, args->desireArray[z], outputNum)) /
                                float(z + 1);
            }
        }

        if (std::isnan(this->fitness) || std::isinf(this->fitness)) {
            this->fitness = -FLT_MAX;
        }
    }

    Individual *Individual::clone() {
        auto duplicate = new Individual();
        duplicate->inputNum = inputNum;
        duplicate->outputNum = outputNum;
        duplicate->args = args;
        duplicate->innovationNum = innovationNum;
        duplicate->nodeSum = nodeSum;
        duplicate->fitness = fitness;

        duplicate->nodeMap = new std::map<uint32_t, Node *>();
        for (auto iter = nodeMap->begin(); iter != nodeMap->end(); ++iter) {
            duplicate->nodeMap->insert(std::make_pair(iter->first, iter->second->clone()));
        }

        duplicate->nodeSlice = new std::vector<uint32_t>();
        duplicate->nodeSlice->assign(nodeSlice->begin(), nodeSlice->end());

        return duplicate;
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
