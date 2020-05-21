/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#include "individual.h"
#include "utils.h"
#include <iostream>
#include <numeric>

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

        for (auto &iter : *nodeMap) {
            delete iter.second;
        }
        delete nodeMap;
    }

    // Initialize a individual.
    void Individual::initialize() {
        nodeMap = new std::map<uint32_t, Node *>();
        nodeSlice = new std::vector<uint32_t>(inputNum + outputNum);

        for (uint32_t i = 0; i < inputNum; ++i) {
            auto temp = nodeSum;
            auto newNode = new Node(temp, 0);
            nodeMap->insert(std::make_pair(temp, newNode));
            (*nodeSlice)[temp] = temp;
            nodeSum++;
        }

        for (uint32_t i = 0; i < outputNum; ++i) {
            auto temp = nodeSum;
            auto newNode = new Node(temp, 2);
            nodeMap->insert(std::make_pair(temp, newNode));
            (*nodeSlice)[temp] = temp;
            nodeSum++;
        }
    }

    // Deep clone of individual.
    Individual *Individual::clone() {
        auto duplicate = new Individual();
        duplicate->inputNum = inputNum;
        duplicate->outputNum = outputNum;
        duplicate->args = args;
        duplicate->innovationNum = innovationNum;
        duplicate->nodeSum = nodeSum;
        duplicate->fitness = fitness;

        duplicate->layers.assign(layers.begin(), layers.end());

        duplicate->nodeMap = new std::map<uint32_t, Node *>();
        for (auto &iter : *nodeMap) {
            duplicate->nodeMap->insert(std::make_pair(iter.first, iter.second->clone()));
        }

        duplicate->nodeSlice = new std::vector<uint32_t>();
        duplicate->nodeSlice->assign(nodeSlice->begin(), nodeSlice->end());

        return duplicate;
    }

    // Return the size of the individual.
    uint32_t Individual::getSize() {
        uint32_t size = 0;
        size += nodeSlice->size();
        for (auto &iter : *nodeMap) {
            size += iter.second->genomeMap->size();
        }
        return size;
    }

    // Set input array => Forward calculation of the individual => Get output array.
    void Individual::forward(std::vector<float> &input, float *output) {
        for (uint32_t i = 0; i < inputNum; ++i) {
            (*nodeMap)[i]->value = input[i];
        }

        uint32_t pointer = 0;

        for (uint32_t i = inputNum; i < nodeSlice->size(); ++i) {
            auto n = (*nodeMap)[(*nodeSlice)[i]];
            float f = 0;

            for (auto &iter : *n->genomeMap) {
                auto g = iter.first;
                auto o = iter.second;
                auto temp = nodeMap->find(g.in);
                if (temp != nodeMap->end()) {
                    f += temp->second->value * o.weight;
                }
            }
            n->value = LycorisUtils::relu(f + n->bias);

            if (n->nodeType == 2) {
                output[pointer] = n->value;
                pointer++;
            }
        }
    }

    // Single threaded version of back propagation.
    void Individual::BP_Single_Thread() {
        this->fitness = 0;
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

                    auto grad = gradient[index] * (n->value > 0 ? 1.0f : 0.2f);

                    for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                        auto p = gradient.find(iter->first.in);
                        if (p != gradient.end()) {
                            gradient[iter->first.in] += grad * iter->second.weight;
                        } else {
                            gradient[iter->first.in] = grad * iter->second.weight;
                        }

                        (*(n->genomeMap))[iter->first].delta_backup = ((*(n->genomeMap))[iter->first].delta_backup * z -
                                                                       args->lr * grad *
                                                                       (*nodeMap)[iter->first.in]->value) /
                                                                      float(z + 1);

                        if (z == args->batchSize - 1) {
                            (*(n->genomeMap))[iter->first].delta = (*(n->genomeMap))[iter->first].delta * 0.9f +
                                                                   (*(n->genomeMap))[iter->first].delta_backup * 0.1f;
                            (*(n->genomeMap))[iter->first].weight += (*(n->genomeMap))[iter->first].delta;
                        }
                    }

                    n->delta_backup = (n->delta_backup * z - args->lr * grad) / float(z + 1);
                    if (z == args->batchSize - 1) {
                        n->delta = n->delta * 0.9f + n->delta_backup * 0.1f;
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

                    auto grad = gradient[index] * (n->value > 0 ? 1.0f : 0.2f);

                    for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                        auto p = gradient.find(iter->first.in);
                        if (p != gradient.end()) {
                            gradient[iter->first.in] += grad * iter->second.weight;
                        } else {
                            gradient[iter->first.in] = grad * iter->second.weight;
                        }

                        (*(n->genomeMap))[iter->first].delta_backup = ((*(n->genomeMap))[iter->first].delta_backup * z -
                                                                       args->lr * grad *
                                                                       (*nodeMap)[iter->first.in]->value) /
                                                                      float(z + 1);

                        if (z == args->batchSize - 1) {
                            (*(n->genomeMap))[iter->first].delta = (*(n->genomeMap))[iter->first].delta * 0.9f +
                                                                   (*(n->genomeMap))[iter->first].delta_backup * 0.1f;
                            (*(n->genomeMap))[iter->first].weight += (*(n->genomeMap))[iter->first].delta;
                        }
                    }

                    n->delta_backup = (n->delta_backup * z - args->lr * grad) / float(z + 1);
                    if (z == args->batchSize - 1) {
                        n->delta = n->delta * 0.9f + n->delta_backup * 0.1f;
                        n->bias += n->delta;
                    }
                }
            }

            if (args->mode == "classify") {
                this->fitness += LycorisUtils::cross_entropy(args->desireArray[z], output, outputNum);
            } else {
                this->fitness -= LycorisUtils::euclidean_distance(output, args->desireArray[z], outputNum);
            }

            if (z == args->batchSize - 1) {
                this->fitness /= args->batchSize;
            }
        }

        if (std::isnan(this->fitness) || std::isinf(this->fitness)) {
            this->fitness = -FLT_MAX;
        }
    }

    // Multi threaded version of back propagation.
    void Individual::BP_Multi_Thread() {
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
            threads.emplace_back(std::thread(&Individual::BP_Multi_Thread_Core, this, start[i], end[i]));
        }
        for (auto &thread : threads) {
            thread.join();
        }

        part = (individualSize) / args->cpuNum;
        for (uint32_t i = 0; i < temp; ++i) {
            start[i] = i * part;
            end[i] = (i + 1) * part;
        }
        start[temp] = temp * part;
        end[temp] = individualSize;

        threads.clear();
        for (uint32_t i = 0; i < args->cpuNum; ++i) {
            threads.emplace_back(
                    std::thread(&Individual::BP_Multi_Thread_Forward, this, start[i], end[i], args->midData));
        }
        for (auto &iter : threads) {
            iter.join();
        }

        uint32_t data_p = 0;

        for (uint32_t j = 0; j < nodeSlice->size(); ++j) {
            auto index = (*nodeSlice)[nodeSlice->size() - 1 - j];
            auto n = (*nodeMap)[index];

            for (auto iter = n->genomeMap->begin(); iter != n->genomeMap->end(); ++iter) {
                (*(n->genomeMap))[iter->first].delta =
                        (*(n->genomeMap))[iter->first].delta * 0.9f + args->midData[data_p++] * 0.1f;
                (*(n->genomeMap))[iter->first].weight += (*(n->genomeMap))[iter->first].delta;
            }

            n->delta = n->delta * 0.9f + args->midData[data_p++] * 0.1f;
            n->bias += n->delta;
        }

        this->fitness = args->midData[data_p];

        if (std::isnan(this->fitness) || std::isinf(this->fitness)) {
            this->fitness = -FLT_MAX;
        }

        delete[] start;
        delete[] end;
    }

    // One of the functor of BP_Multi_Thread().
    void Individual::BP_Multi_Thread_Core(uint32_t start, uint32_t end) {
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

                    auto grad = gradient[index] * (n->value > 0 ? 1.0f : 0.2f);

                    for (auto &iter : *n->genomeMap) {
                        auto p = gradient.find(iter.first.in);
                        if (p != gradient.end()) {
                            gradient[iter.first.in] += grad * iter.second.weight;
                        } else {
                            gradient[iter.first.in] = grad * iter.second.weight;
                        }

                        args->batchData[z][data_p++] = 0 - ci->args->lr * grad * (*ci->nodeMap)[iter.first.in]->value;
                    }

                    args->batchData[z][data_p++] = 0 - ci->args->lr * grad;
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

                    auto grad = gradient[index] * (n->value > 0 ? 1.0f : 0.2f);

                    for (auto &iter : *n->genomeMap) {
                        auto p = gradient.find(iter.first.in);
                        if (p != gradient.end()) {
                            gradient[iter.first.in] += grad * iter.second.weight;
                        } else {
                            gradient[iter.first.in] = grad * iter.second.weight;
                        }

                        args->batchData[z][data_p++] = 0 - ci->args->lr * grad * (*ci->nodeMap)[iter.first.in]->value;
                    }

                    args->batchData[z][data_p++] = 0 - ci->args->lr * grad;
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

    // One of the functor of BP_Multi_Thread().
    void Individual::BP_Multi_Thread_Forward(uint32_t start, uint32_t end, float *midData) {
        for (uint32_t i = start; i < end; ++i) {
            float temp_data = 0.0f;

            for (uint32_t j = 0; j < args->batchSize; ++j) {
                temp_data += args->batchData[j][i];
            }

            temp_data /= args->batchSize;
            midData[i] = temp_data;
        }
    }

    /*
     * Get a vector of nodes in a specific layer of this neural network.
     * The parameter pos starts at index 0.
     */
    std::vector<float> Individual::getHiddenLayer(uint32_t pos) {
        std::vector<float> ret;

        auto pre = std::accumulate(layers.begin(), layers.begin() + pos, uint32_t(0));
        for (uint32_t i = pre; i < pre + layers[pos]; ++i) {
            ret.push_back((*nodeMap)[(*nodeSlice)[i]]->value);
        }

        return ret;
    }

}
