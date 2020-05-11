/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#include "lycoris.h"
#include <algorithm>
#include <iostream>
#include <fstream>

namespace LycorisNet {

    Lycoris::Lycoris(uint32_t capacity, uint32_t inputDim, uint32_t outputDim, const std::string &mode) {
        this->capacity = capacity;
        this->inputNum = inputDim;
        this->outputNum = outputDim;

        args = new Args();
        if (mode != "predict" && mode != "classify") {
            std::cout << R"(The mode is invalid. (Please input "predict" or "classify"))" << std::endl;
            exit(1);
        }
        args->mode = mode;
        args->weightB = sqrtf(6.0f / inputNum) + 0.000001f;
        args->weightA = 0 - args->weightB;
    }

    Lycoris::~Lycoris() {
        checkFirstRun();

        delete args;

        for (auto &iter : *individualList) {
            delete iter;
        }

        delete individualList;
    }

    // Preheating process of the neural network cluster.
    void Lycoris::preheat(uint32_t num_of_nodes, uint32_t num_of_connections, uint32_t depth) {
        if (args->preheatFlag) {
            args->preheatFlag = false;
        } else {
            std::cout << R"(The function "preheat" can only be executed once.)" << std::endl;
            exit(8);
        }

        if (!args->evolveFlag) {
            std::cout << "The function preheat() cannot be executed after evolve()." << std::endl;
            exit(10);
        }

        if (depth < 2 || depth > num_of_nodes + 2) {
            std::cout << R"(The parameter "depth" is invalid.)" << std::endl;
            exit(4);
        }

        args->depth = depth;
        checkFirstRun();

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
            threads.emplace_back(
                    std::thread(&Lycoris::preheatCore, this, start[i], end[i], num_of_nodes, num_of_connections));
        }
        for (auto &thread : threads) {
            thread.join();
        }

        delete[] start;
        delete[] end;
    }

    // Evolve the neural network cluster.
    void Lycoris::evolve(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire) {
        if (args->evolveFlag) {
            args->evolveFlag = false;
        }

        if (input.size() != desire.size()) {
            std::cout << "The input data and the desire data do not match!" << std::endl;
            exit(7);
        }

        args->inputArray = input;
        args->desireArray = desire;
        args->batchSize = input.size();

        runLycoris();
    }

    // Fit all neural networks in the neural network cluster.
    void Lycoris::fit(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire) {
        if (capacity > 1) {
            fitAll(input, desire);
        } else {
            if (input.size() != desire.size()) {
                std::cout << "The input data and the desire data do not match!" << std::endl;
                exit(7);
            }

            args->inputArray = input;
            args->desireArray = desire;
            auto batchSize = input.size();
            auto individualSize = best->getSize() + 1;

            if (!args->batchFlag) {
                if (batchSize != args->batchSize_ || individualSize != args->individualSize_) {
                    for (uint32_t i = 0; i < args->batchSize_; ++i) {
                        delete[] args->batchData[i];
                    }
                    delete[] args->batchData;

                    delete[] args->midData;

                    args->batchData = new float *[batchSize];

                    for (uint32_t i = 0; i < batchSize; ++i) {
                        args->batchData[i] = new float[individualSize];
                    }

                    args->midData = new float[individualSize];
                }
            }
            args->batchSize = batchSize;
            args->batchSize_ = batchSize;
            args->individualSize_ = individualSize;

            checkFirstRun();

            best->BP_Multi_Thread();
        }
    }

    // Keep only the best one in the neural network cluster.
    void Lycoris::enrich() {
        if (capacity > 1) {
            checkFirstRun();

            uint32_t totalLength = individualList->size();
            if (totalLength == 0) {
                std::cout << "All died." << std::endl;
                exit(1);
            }

            std::vector<SortFitness> sortList(totalLength);
            for (uint32_t i = 0; i < totalLength; ++i) {
                sortList[i] = SortFitness((*individualList)[i]->fitness, i);
            }
            std::sort(sortList.begin(), sortList.end(), LycorisUtils::compareFitness);
            auto last = sortList[totalLength - 1];
            best = (*individualList)[last.individualNum];

            auto newIndividualList = new std::vector<Individual *>();

            auto z = totalLength;
            for (; z > totalLength - 1; --z) {
                if (z == 0) { // This needs elegant repairs.
                    break;
                }
                auto temp = sortList[z - 1];
                auto tempIndividual = (*individualList)[temp.individualNum];
                newIndividualList->push_back(tempIndividual);
            }

            if (totalLength > 1) {
                for (uint32_t i = 0; i < totalLength - 1; ++i) {
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

            this->capacity = 1;
        }
    }

    // Forward Computing of the best individual.
    std::vector<float> Lycoris::compute(std::vector<float> &input) {
        if (input.size() != inputNum) {
            std::cout << "The input data is not proper!" << std::endl;
            exit(6);
        }

        checkFirstRun();

        float output[outputNum];
        best->forward(input, output);

        if (args->mode == "classify") {
            LycorisUtils::softmax(output, outputNum);
        }

        std::vector<float> ret(output, output + outputNum);
        return ret;
    }

    // Parallel forward Computing of the best individual.
    std::vector<std::vector<float> > Lycoris::computeBatch(std::vector<std::vector<float> > &input) {
        if (input.empty() || input[0].size() != inputNum) {
            std::cout << "The input data is not proper!" << std::endl;
            exit(6);
        }

        checkFirstRun();

        data_for_computeBatch = &input;

        auto start = new uint32_t[args->cpuNum];
        auto end = new uint32_t[args->cpuNum];
        auto batchLength = uint32_t(input.size());
        auto part = batchLength / args->cpuNum;
        auto temp = args->cpuNum - 1;
        for (uint32_t i = 0; i < temp; ++i) {
            start[i] = i * part;
            end[i] = (i + 1) * part;
        }
        start[temp] = temp * part;
        end[temp] = batchLength;

        auto output = new float *[batchLength];
        for (uint32_t i = 0; i < batchLength; ++i) {
            output[i] = new float[outputNum];
        }
        std::vector<std::vector<float> > ret(batchLength);

        std::vector<std::thread> threads;
        for (uint32_t i = 0; i < args->cpuNum; ++i) {
            threads.emplace_back(std::thread(&Lycoris::computeBatchCore, this, start[i], end[i], output));
        }
        for (auto &thread : threads) {
            thread.join();
        }

        for (uint32_t i = 0; i < batchLength; ++i) {
            std::vector<float> sub_vector(output[i], output[i] + outputNum);
            ret[i] = sub_vector;
        }

        delete[] start;
        delete[] end;
        for (uint32_t i = 0; i < batchLength; ++i) {
            delete[] output[i];
        }
        delete[] output;

        return ret;
    }

    // Resize the capacity of the neural network cluster.
    void Lycoris::resize(uint32_t num) {
        if (num < 1) {
            std::cout << "Invalid parameter." << std::endl;
            exit(13);
        }

        checkFirstRun();

        auto temp1 = uint32_t(float(num) / (1 + args->mutateOdds));
        auto temp2 = uint32_t(individualList->size());

        if (temp1 < temp2) {
            uint32_t totalLength = temp2;
            if (totalLength == 0) {
                std::cout << "All died." << std::endl;
                exit(2);
            }

            std::vector<SortFitness> sortList(totalLength);
            for (uint32_t i = 0; i < totalLength; ++i) {
                sortList[i] = SortFitness((*individualList)[i]->fitness, i);
            }
            std::sort(sortList.begin(), sortList.end(), LycorisUtils::compareFitness);
            auto last = sortList[totalLength - 1];
            best = (*individualList)[last.individualNum];

            auto newIndividualList = new std::vector<Individual *>();
            auto newLength = (temp1 == 0 ? 1 : temp1);

            auto z = totalLength;
            for (; z > totalLength - newLength; --z) {
                if (z == 0) { // This needs elegant repairs.
                    break;
                }
                auto temp = sortList[z - 1];
                auto tempIndividual = (*individualList)[temp.individualNum];
                newIndividualList->push_back(tempIndividual);
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
        } else if (temp1 > temp2) {
            auto start = new uint32_t[args->cpuNum];
            auto end = new uint32_t[args->cpuNum];
            oldLength = temp2;
            auto mutateLength = temp1 - temp2;
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
            for (auto &thread : threads) {
                thread.join();
            }

            for (uint32_t i = 0; i < mutateLength; ++i) {
                individualList->push_back(tempList[i]);
            }

            delete[] start;
            delete[] end;
        }

        this->capacity = num;
    }

    // Turn on memory-limit.
    void Lycoris::openMemLimit(uint32_t size) {
        args->limitSize = size * 9 / 10;
        args->memLimitFlag = true;
    }

    // Turn off memory-limit.
    void Lycoris::closeMemLimit() {
        args->memLimitFlag = false;
        args->memOverFlag = false;
    }

    // Export the current trained model.
    void Lycoris::saveModel(const std::string &path) {
        checkFirstRun();

        std::ofstream outfile;
        outfile.open(path);

        outfile << std::to_string(best->inputNum) << " ";
        outfile << std::to_string(best->outputNum) << " ";
        outfile << std::to_string(best->innovationNum) << " ";
        outfile << std::to_string(best->nodeSum) << " ";
        outfile << std::to_string(best->fitness) << " ";

        outfile << std::to_string(best->nodeSlice->size()) << " ";
        for (unsigned int &iter : *best->nodeSlice) {
            outfile << std::to_string(iter) << " ";
        }

        outfile << std::to_string(best->nodeMap->size()) << " ";
        for (auto &iter : *best->nodeMap) {
            outfile << std::to_string(iter.first) << " ";
            outfile << std::to_string(iter.second->nodeNum) << " ";
            outfile << std::to_string(iter.second->nodeType) << " ";
            outfile << std::to_string(iter.second->value) << " ";
            outfile << std::to_string(iter.second->bias) << " ";

            outfile << std::to_string(iter.second->genomeMap->size()) << " ";
            for (auto &it : *iter.second->genomeMap) {
                outfile << std::to_string(it.first.in) << " ";
                outfile << std::to_string(it.first.out) << " ";
                outfile << std::to_string(it.second.weight) << " ";
                outfile << std::to_string(it.second.innovationNum) << " ";
            }
        }

        outfile << std::to_string(best->layers.size()) << " ";
        for (unsigned int &iter : best->layers) {
            outfile << std::to_string(iter) << " ";
        }

        outfile << args->mode;

        outfile.close();
    }

    // Set p1 to p4 in the class Args.
    void Lycoris::setMutateArgs(std::vector<float> &p) {
        if (p.size() != 4 || fabsf(p[0] + p[1] + p[2] + p[3] - 1.0f) > 0.001) {
            std::cout << "The input is invalid!" << std::endl;
            exit(5);
        }

        args->p1 = p[0];
        args->p2 = p[1];
        args->p3 = p[2];
        args->p4 = p[3];
        args->p1B = p[0];
        args->p2B = p[1];
        args->p3B = p[2];
        args->p4B = p[3];
    }

    // Set the odds of mutating.
    void Lycoris::setMutateOdds(float odds) {
        if (odds < 0) {
            std::cout << "The parameter cannot be set to smaller than zero." << std::endl;
            exit(3);
        }
        args->mutateOdds = odds;

        resize(capacity);
    }

    // Set the number of worker threads to train the model.
    void Lycoris::setCpuCores(uint32_t num) {
        args->cpuNum = num;
    }

    // Set the learning rate.
    void Lycoris::setLR(float lr) {
        args->lr = lr;
    }

    // Returns the size of the best individual.
    uint32_t Lycoris::getSize() {
        checkFirstRun();

        return best->getSize();
    }

    // Returns the input dimension.
    uint32_t Lycoris::getInputDim() {
        return inputNum;
    }

    // Returns the output dimension.
    uint32_t Lycoris::getOutputDim() {
        return outputNum;
    }

    // Returns capacity of Lycoris.
    uint32_t Lycoris::getCapacity() {
        return capacity;
    }

    // Returns the loss.
    float Lycoris::getLoss() {
        if (args->firstRun) {
            std::cout << "The function cannot be executed before initialization." << std::endl;
            exit(9);
        }

        return 0 - best->fitness;
    }

    // Returns mode of Lycoris (classify or predict).
    std::string Lycoris::getMode() {
        return args->mode;
    }

    // Returns the number of nodes in each layer of the neural network.
    std::vector<uint32_t> Lycoris::getLayers() {
        if (args->preheatFlag) {
            std::cout << "The preheat() is not executed." << std::endl;
            exit(11);
        }

        if (!args->evolveFlag) {
            std::cout << "The evolve() has been executed." << std::endl;
            exit(12);
        }

        return best->layers;
    }

    /*
     * Get a vector of nodes in a specific layer of the best
     * individual. The parameter pos starts at index 0.
     */
    std::vector<float> Lycoris::getHiddenLayer(uint32_t pos) {
        if (args->preheatFlag) {
            std::cout << "The preheat() is not executed." << std::endl;
            exit(11);
        }

        if (!args->evolveFlag) {
            std::cout << "The evolve() has been executed." << std::endl;
            exit(12);
        }

        if (pos < 0 || pos >= args->depth) {
            std::cout << "Invalid parameter." << std::endl;
            exit(13);
        }

        return best->getHiddenLayer(pos);
    }

    // Fit all neural networks in the neural network cluster.
    void Lycoris::fitAll(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire) {
        if (input.size() != desire.size()) {
            std::cout << "The input data and the desire data do not match!" << std::endl;
            exit(7);
        }

        args->inputArray = input;
        args->desireArray = desire;
        args->batchSize = input.size();

        checkFirstRun();

        backPropagation();
    }

    // Mutating.
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
        for (auto &thread : threads) {
            thread.join();
        }

        for (uint32_t i = 0; i < mutateLength; ++i) {
            individualList->push_back(tempList[i]);
        }

        delete[] start;
        delete[] end;
    }

    // The functor of mutate().
    void Lycoris::mutateCore(uint32_t start, uint32_t end) {
        for (uint32_t i = start; i < end; ++i) {
            tempList[i] = args->utils->mutateIndividual(
                    *((*individualList)[args->utils->LycorisRandomUint32_t(oldLength)]));
        }
    }

    // BP_Single_Thread() is invoked in every individual of the neural network cluster.
    void Lycoris::backPropagation() {
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
            threads.emplace_back(std::thread(&Lycoris::backPropagationCore, this, start[i], end[i]));
        }
        for (auto &thread : threads) {
            thread.join();
        }

        delete[] start;
        delete[] end;
    }

    // The functor of backPropagation().
    void Lycoris::backPropagationCore(uint32_t start, uint32_t end) {
        for (uint32_t i = start; i < end; ++i) {
            (*individualList)[i]->BP_Single_Thread();
        }
    }

    // All the individuals in the neural network cluster are sorted and the better ones are selected.
    void Lycoris::chooseElite() {
        uint32_t totalLength = individualList->size();
        if (totalLength == 0) {
            std::cout << "All died." << std::endl;
            exit(2);
        }

        std::vector<SortFitness> sortList(totalLength);
        for (uint32_t i = 0; i < totalLength; ++i) {
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

    // Main part of evolve().
    void Lycoris::runLycoris() {
        checkFirstRun();

        // Mutating.
        mutate();
        // Forward calculation.
        backPropagation();
        // Sorting and choosing some individuals with higher fitness.
        chooseElite();

        // Memory exceeds the limit.
        if (args->memOverFlag) {
            args->p1 = 0;
            args->p3 = 0;
            auto temp = args->p2 + args->p4;
            args->p2 /= temp;
            args->p4 /= temp;
        } else {
            args->p1 = args->p1B;
            args->p2 = args->p2B;
            args->p3 = args->p3B;
            args->p4 = args->p4B;
        }
    }

    // The functor of preheat().
    void Lycoris::preheatCore(uint32_t start, uint32_t end, uint32_t num_of_nodes, uint32_t num_of_connections) {
        for (uint32_t i = start; i < end; ++i) {
            LycorisNet::LycorisUtils::addHiddenNodes(*((*individualList)[i]), num_of_nodes);
            args->utils->addConnections(*((*individualList)[i]), num_of_connections);
        }
    }

    // The functor of computeBatch().
    void Lycoris::computeBatchCore(uint32_t start, uint32_t end, float **output) {
        auto temp = best->clone();

        for (uint32_t i = start; i < end; ++i) {
            temp->forward((*data_for_computeBatch)[i], output[i]);

            if (args->mode == "classify") {
                LycorisUtils::softmax(output[i], outputNum);
            }
        }

        delete temp;
    }

    // Check if the individualList is initialized.
    void Lycoris::checkFirstRun() {
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
    }

    // Import the pre-trained model.
    Lycoris *loadModel(const std::string &path, uint32_t capacity) {
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
                o.innovationNum = uint32_t(std::stoul(data[pointer]));
                pointer++;
                (*(n->genomeMap))[g] = o;
            }

            (*(source->nodeMap))[key] = n;
        }

        auto layerSize = uint32_t(std::stoul(data[pointer]));
        pointer++;
        for (uint32_t i = 0; i < layerSize; ++i) {
            source->layers.push_back(uint32_t(std::stoul(data[pointer])));
            pointer++;
        }

        auto mode = data[pointer];

        auto radiata = new Lycoris(capacity, source->inputNum, source->outputNum, mode);
        radiata->args->firstRun = false;
        radiata->args->preheatFlag = false;
        source->args = radiata->args;

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

    // Import the pre-trained model via string.
    Lycoris *loadViaString(const std::string &model, uint32_t capacity) {
        auto data = LycorisUtils::split(model);
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
                o.innovationNum = uint32_t(std::stoul(data[pointer]));
                pointer++;
                (*(n->genomeMap))[g] = o;
            }

            (*(source->nodeMap))[key] = n;
        }

        auto layerSize = uint32_t(std::stoul(data[pointer]));
        pointer++;
        for (uint32_t i = 0; i < layerSize; ++i) {
            source->layers.push_back(uint32_t(std::stoul(data[pointer])));
            pointer++;
        }

        auto mode = data[pointer];

        auto radiata = new Lycoris(capacity, source->inputNum, source->outputNum, mode);
        radiata->args->firstRun = false;
        radiata->args->preheatFlag = false;
        source->args = radiata->args;

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

}
