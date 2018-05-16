#include <algorithm>
#include "utils.h"

uint32_t sort1(Individual &in, std::vector<float> &ret1, std::vector<uint32_t> &ret2) {
    std::vector<bool> temp1(in.innovationNum, false);
    float temp2[in.innovationNum];
    uint32_t temp3[in.innovationNum];

    uint32_t length = 0;
    for (auto iter = in.nodeMap->begin(); iter != in.nodeMap->end(); ++iter) {
        auto genomeMap = iter->second->genomeMap;
        length += genomeMap->size();
        for (auto it = genomeMap->begin(); it != genomeMap->end(); ++it) {
            auto num = it->second.innovationNum;
            temp1[num] = true;
            temp2[num] = it->second.weight;
            temp3[num] = num;
        }
    }

    if (length != 0) {
        ret1.resize(length);
        ret2.resize(length);
        uint32_t count = 0;
        for (int i = 0; i < temp1.size(); ++i) {
            if (temp1[i]) {
                ret1[count] = temp2[i];
                ret2[count] = temp3[i];
                count++;
            }
        }
    }

    return length;
}

uint32_t sort2(Individual &in, std::vector<Gen> &ret1, std::vector<Ome> &ret2) {
    std::vector<bool> temp1(in.innovationNum, false);
    Gen temp2[in.innovationNum];
    Ome temp3[in.innovationNum];

    uint32_t length = 0;
    for (auto iter = in.nodeMap->begin(); iter != in.nodeMap->end(); ++iter) {
        auto genomeMap = iter->second->genomeMap;
        length += genomeMap->size();
        for (auto it = genomeMap->begin(); it != genomeMap->end(); ++it) {
            auto num = it->second.innovationNum;
            temp1[num] = true;
            temp2[num] = it->first;
            temp3[num] = it->second;
        }
    }

    if (length != 0) {
        ret1.resize(length);
        ret2.resize(length);
        uint32_t count = 0;
        for (int i = 0; i < temp1.size(); ++i) {
            if (temp1[i]) {
                ret1[count] = temp2[i];
                ret2[count] = temp3[i];
                count++;
            }
        }
    }

    return length;
}

float distance(Individual &in1, Individual &in2) {
    float d;
    uint32_t DE = 0;
    std::vector<float> w1;
    std::vector<uint32_t> i1;
    std::vector<float> w2;
    std::vector<uint32_t> i2;

    auto len1 = sort1(in1, w1, i1);
    auto len2 = sort1(in2, w2, i2);
    uint32_t N = 0;
    if (len1 > len2) {
        N = len1;
    } else {
        N = len2;
    }
    if (N == 0) {
        return 0;
    }
    float W = 0;
    uint32_t countW = 0;
    uint32_t point1 = 0;
    uint32_t point2 = 0;

    while (true) {
        if (point1 == len1 || point2 == len2) {
            break;
        }

        if (i1[point1] == i2[point2]) {
            auto temp = (w1[point1] - w2[point2]);
            W += temp > 0 ? temp : (0 - temp);
            countW++;
            point1++;
            point2++;
        } else if (i1[point1] > i2[point2]) {
            DE++;
            point2++;
        } else {
            DE++;
            point1++;
        }
    }

    DE += len1 + len2 - point1 - point2;
    d = (in1.args->c1 * DE) / N + (in1.args->c2 * W) / countW;

    return d;
}

Individual *mateIndividual(Individual &in1, Individual &in2) {
    auto offspring = new Individual();
    offspring->inputNum = in1.inputNum;
    offspring->outputNum = in1.outputNum;
    offspring->args = in1.args;
    offspring->nodeMap = new std::map<uint32_t, Node *>();

    std::map<uint32_t, bool> tempMap;
    std::map<uint32_t, bool> duplicateNodes;

    for (auto iter = in1.nodeMap->begin(); iter != in1.nodeMap->end(); ++iter) {
        auto k = iter->first;
        tempMap[k] = true;
        duplicateNodes[k] = false;
    }

    for (auto iter = in2.nodeMap->begin(); iter != in2.nodeMap->end(); ++iter) {
        auto k = iter->first;
        if (tempMap.find(k) != tempMap.end()) {
            duplicateNodes[k] = true;
        } else {
            tempMap[k] = true;
            duplicateNodes[k] = false;
        }
    }

    std::vector<uint32_t> tempSlice(tempMap.size());
    uint32_t tempCount = 0;
    for (auto iter = tempMap.begin(); iter != tempMap.end(); ++iter) {
        tempSlice[tempCount] = iter->first;
        tempCount++;
    }

    auto basicNodeSum = in1.inputNum + in1.outputNum;
    for (auto iter = tempSlice.begin(); iter != tempSlice.end(); ++iter) {
        Node *n;
        auto v = *iter;

        if (v < offspring->inputNum) {
            n = new Node(v, 0);
        } else if (v >= offspring->inputNum && v < basicNodeSum) {
            n = new Node(v, 2);
        } else {
            n = new Node(v, 1);
            if (in1.nodeMap->find(v) != in1.nodeMap->end()) {
                n->bias = (*(in1.nodeMap))[v]->bias;
            } else {
                n->bias = (*(in2.nodeMap))[v]->bias;
            }
        }

        if (duplicateNodes[v]) {
            if (LycorisRandomFloat(0, 1) < 0.5) {
                n->bias = (*(in1.nodeMap))[v]->bias;
            } else {
                n->bias = (*(in2.nodeMap))[v]->bias;
            }
        }

        (*(offspring->nodeMap))[v] = n;
    }

    std::vector<Gen> g1;
    std::vector<Ome> o1;
    std::vector<Gen> g2;
    std::vector<Ome> o2;

    auto len1 = sort2(in1, g1, o1);
    auto len2 = sort2(in2, g2, o2);
    uint32_t point1 = 0;
    uint32_t point2 = 0;

    while (true) {
        if (point1 == len1 || point2 == len2) {
            break;
        }

        if (o1[point1].innovationNum == o2[point2].innovationNum) {
            if (LycorisRandomFloat(0, 1) < 0.5) {
                (*(*(offspring->nodeMap))[g1[point1].out]->genomeMap)[g1[point1]] = o1[point1];
            } else {
                (*(*(offspring->nodeMap))[g2[point2].out]->genomeMap)[g2[point2]] = o2[point2];
            }
            point1++;
            point2++;
        } else if (o1[point1].innovationNum > o2[point2].innovationNum) {
            (*(*(offspring->nodeMap))[g2[point2].out]->genomeMap)[g2[point2]] = o2[point2];
            point2++;
        } else {
            (*(*(offspring->nodeMap))[g1[point1].out]->genomeMap)[g1[point1]] = o1[point1];
            point1++;
        }
    }

    for (int i = point1; i < len1; ++i) {
        (*(*(offspring->nodeMap))[g1[i].out]->genomeMap)[g1[i]] = o1[i];
    }

    for (int i = point2; i < len2; ++i) {
        (*(*(offspring->nodeMap))[g2[i].out]->genomeMap)[g2[i]] = o2[i];
    }

    std::map<uint32_t, uint32_t> inDegree;
    for (auto iter = offspring->nodeMap->begin(); iter != offspring->nodeMap->end(); ++iter) {
        inDegree[iter->first] = uint32_t(iter->second->genomeMap->size());
    }
    std::vector<uint32_t> next;
    for (auto iter = inDegree.begin(); iter != inDegree.end(); ++iter) {
        if (iter->second == 0) {
            next.push_back(iter->first);
            inDegree.erase(iter->first);
        }
    }
    uint32_t slicePointer = 0;

    while (true) {
        if (next.size() == 0) {
            break;
        }
        auto headValue = next.front();
        tempSlice[slicePointer] = headValue;
        slicePointer++;
        next.erase(next.begin());

        for (auto iter = offspring->nodeMap->begin(); iter != offspring->nodeMap->end(); ++iter) {
            auto v = iter->second;
            for (auto it = v->genomeMap->begin(); it != v->genomeMap->end(); ++it) {
                if (it->first.in == headValue) {
                    auto outNum = v->nodeNum;
                    if (inDegree[outNum] == 1) {
                        inDegree.erase(outNum);
                        next.push_back(outNum);
                    } else {
                        inDegree[outNum] = inDegree[outNum] - 1;
                    }
                    break;
                }
            }
        }
    }
    for (auto iter = inDegree.begin(); iter != inDegree.end(); ++iter) {
        tempSlice[slicePointer] = iter->first;
        slicePointer++;
    }

    offspring->nodeSlice = new std::vector<uint32_t>(tempSlice.size());
    for (uint32_t j = 0; j < in1.inputNum; ++j) {
        (*(offspring->nodeSlice))[j] = j;
    }
    auto point = in1.inputNum;
    for (auto iter = tempSlice.begin(); iter != tempSlice.end(); ++iter) {
        if (*iter >= in1.inputNum) {
            (*(offspring->nodeSlice))[point] = *iter;
            point++;
        }
    }

    if (in1.nodeSum > in2.nodeSum) {
        offspring->nodeSum = in1.nodeSum;
    } else {
        offspring->nodeSum = in2.nodeSum;
    }

    if (in1.innovationNum > in2.innovationNum) {
        offspring->innovationNum = in1.innovationNum;
    } else {
        offspring->innovationNum = in2.innovationNum;
    }

    return offspring;
}

Individual *mutateIndividual(Individual &in) {
    auto offspring = in.clone();

    for (uint32_t z = 0; z < in.args->mutateTime; ++z) {
        auto ran = LycorisRandomFloat(0, 1);

        if (ran < in.args->p1) {
            auto nodeOld = (*(offspring->nodeMap))[(*(offspring->nodeSlice))[LycorisRandomUint32_t(
                    uint32_t(offspring->nodeSlice->size()))]];

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
                }

                omeOld.isEnable = false;
                auto n = new Node(offspring->nodeSum, 1);
                n->initializeBias(LycorisRandomFloat(in.args->biasA, in.args->biasB));
                offspring->nodeSum++;
                Gen g1(genOld.in, n->nodeNum);
                Ome o1(1.0, true, offspring->innovationNum);
                offspring->innovationNum++;
                Gen g2(n->nodeNum, genOld.out);
                Ome o2(omeOld.weight, true, offspring->innovationNum);
                offspring->innovationNum++;
                (*(n->genomeMap))[g1] = o1;
                (*(nodeOld->genomeMap))[genOld] = omeOld;
                (*(nodeOld->genomeMap))[g2] = o2;
                (*(offspring->nodeMap))[n->nodeNum] = n;

                auto iter = std::find(offspring->nodeSlice->begin(), offspring->nodeSlice->end(), genOld.out);
                offspring->nodeSlice->insert(iter, n->nodeNum);
            }
        } else if (ran >= in.args->p1 && ran < in.args->p1 + in.args->p2) {
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index = LycorisRandomUint32_t(length);
            auto sliceIndex = (*(offspring->nodeSlice))[index];
            if ((*(offspring->nodeMap))[sliceIndex]->nodeType == 1) {
                for (int i = index + 1; i < length; ++i) {
                    auto outputNum = (*(offspring->nodeSlice))[i];
                    auto n = (*(offspring->nodeMap))[outputNum];
                    n->genomeMap->erase(Gen(sliceIndex, outputNum));
                }
                auto temp = (*(offspring->nodeMap))[sliceIndex];
                offspring->nodeMap->erase(sliceIndex);
                offspring->nodeSlice->erase(offspring->nodeSlice->begin() + index);
                delete temp;
            }
        } else if (ran >= in.args->p1 + in.args->p2 && ran < in.args->p1 + in.args->p2 + in.args->p3) {
            auto length = uint32_t(offspring->nodeSlice->size());
            auto index1 = LycorisRandomUint32_t(length);
            auto index2 = index1 + LycorisRandomUint32_t(length - index1);

            if (index1 != index2) {
                auto inputNum = (*(offspring->nodeSlice))[index1];
                auto outputNum = (*(offspring->nodeSlice))[index2];
                auto inputNode = (*(offspring->nodeMap))[inputNum];
                auto outputNode = (*(offspring->nodeMap))[outputNum];

                if (!((inputNode->nodeType == 0 && outputNode->nodeType == 0) ||
                      (inputNode->nodeType == 2 && outputNode->nodeType == 2))) {
                    Gen g(inputNum, outputNum);
                    if (outputNode->genomeMap->find(g) == outputNode->genomeMap->end()) {
                        Ome o(LycorisRandomFloat(0, 1) * (in.args->weightB - in.args->weightA) + in.args->weightA, true,
                              offspring->innovationNum);
                        offspring->innovationNum++;
                        (*(outputNode->genomeMap))[g] = o;
                    }
                }
            }
        } else if (ran >= in.args->p1 + in.args->p2 + in.args->p3 &&
                   ran < in.args->p1 + in.args->p2 + in.args->p3 + in.args->p4) {
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
                }
                n->genomeMap->erase(g);
            }
        } else if (ran >= in.args->p1 + in.args->p2 + in.args->p3 + in.args->p4 &&
                   ran < in.args->p1 + in.args->p2 + in.args->p3 + in.args->p4 + in.args->p5) {
            auto n = new Node(offspring->nodeSum, 1);
            n->initializeBias(LycorisRandomFloat(in.args->biasA, in.args->biasB));
            offspring->nodeSum++;
            (*(offspring->nodeMap))[n->nodeNum] = n;
            auto index = LycorisRandomUint32_t(uint32_t(offspring->nodeSlice->size()) - offspring->inputNum) +
                         offspring->inputNum;
            offspring->nodeSlice->insert(offspring->nodeSlice->begin() + index, n->nodeNum);
        } else {
            auto index = LycorisRandomUint32_t(uint32_t(offspring->nodeSlice->size()) - offspring->inputNum) +
                         offspring->inputNum;
            auto n = (*(offspring->nodeMap))[(*(offspring->nodeSlice))[index]];
            n->bias = LycorisRandomFloat(in.args->biasA, in.args->biasB);
        };
    }

    return offspring;
}

SortFitness::SortFitness() {

}

SortFitness::SortFitness(float fitness, uint32_t specieNum, uint32_t individualNum) {
    this->fitness = fitness;
    this->specieNum = specieNum;
    this->individualNum = individualNum;
}

SortFitness::~SortFitness() {

}
