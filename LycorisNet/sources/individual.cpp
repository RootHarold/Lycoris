#include "individual.h"
#include "utils.h"

Individual::Individual() {

}

Individual::Individual(unsigned inputNum, unsigned outputNum, Args *args) {
    this->inputNum = inputNum;
    this->outputNum = outputNum;
    this->args = args;
    nodeSum = 0;
    innovationNum = 0;
    initialize();
}

Individual::~Individual() {
    delete nodeSlice;

    std::map<unsigned, Node *>::iterator iter;
    for (iter = nodeMap->begin(); iter != nodeMap->end(); iter++) {
        delete iter->second;
    }
    delete nodeMap;
}

void Individual::initialize() {
    nodeMap = new std::map<unsigned, Node *>();
    nodeSlice = new std::vector<unsigned>(inputNum + outputNum);

    for (unsigned i = 0; i < inputNum; ++i) {
        auto temp = nodeSum;
        auto newNode = new Node(temp, 0);
        newNode->initializeBias(LycorisRandomFloat(args->biasA, args->biasB));
        (*nodeMap)[temp] = newNode;
        (*nodeSlice)[temp] = temp;
        nodeSum++;
    }

    for (unsigned i = 0; i < outputNum; ++i) {
        auto temp = nodeSum;
        auto newNode = new Node(temp, 2);
        newNode->initializeBias(LycorisRandomFloat(args->biasA, args->biasB));
        (*nodeMap)[temp] = newNode;
        (*nodeSlice)[temp] = temp;
        nodeSum++;
    }
}

void Individual::setInput(float *input) {
    for (unsigned i = 0; i < inputNum; ++i) {
        (*nodeMap)[i]->value = input[i];
    }
}

float *Individual::getOutput() {
    float *output = new float[outputNum];
    unsigned pointer = 0;
    for (unsigned i = inputNum; i < nodeSlice->size(); ++i) {
        auto temp = (*nodeMap)[(*nodeSlice)[i]];
        if (temp->nodeType == 2) {
            output[pointer] = temp->value;
            pointer++;
        }
    }
    return output;
}

void Individual::forward() {
    std::map<unsigned, bool> clean;

    for (unsigned i = inputNum; i < nodeSlice->size(); ++i) {
        auto index = (*nodeSlice)[i];
        auto n = (*nodeMap)[index];
        float f = 0;

        if (n->genomeMap->size() == 0) {
            if (n->nodeType == 1) {
                clean[index] = true;
            }
        } else {
            std::map<Gen, Ome>::iterator iter;
            for (iter = n->genomeMap->begin(); iter != n->genomeMap->end(); iter++) {
                auto g = iter->first;
                auto o = iter->second;
                if (o.isEnable) {
                    f += (*nodeMap)[g.in]->value * o.weight;
                }
            }
            n->value = args->activateFunc(f + n->bias);
        }
    }

    if (LycorisRandomFloat(0, 1) < args->cleanOdds && clean.size() != 0) {
        std::map<unsigned, bool>::iterator iter;
        for (iter = clean.begin(); iter != clean.end(); iter++) {
            auto key = iter->first;
            auto temp = (*nodeMap)[key];
            nodeMap->erase(key);
            delete temp;
        }

        for (std::vector<unsigned>::iterator iter = nodeSlice->begin(); iter != nodeSlice->end();) {
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

    dulplicate->nodeMap = new std::map<unsigned, Node *>();
    std::map<unsigned, Node *>::iterator iter;
    for (iter = nodeMap->begin(); iter != nodeMap->end(); iter++) {
        (*(dulplicate->nodeMap))[iter->first] = iter->second->clone();
    }

    dulplicate->nodeSlice = new std::vector<unsigned>();
    dulplicate->nodeSlice->assign(nodeSlice->begin(), nodeSlice->end());

    return dulplicate;
}

unsigned Individual::getSize() {
    unsigned size = 0;
    size += nodeSlice->size();
    std::map<unsigned, Node *>::iterator iter;
    for (iter = nodeMap->begin(); iter != nodeMap->end(); iter++) {
        size += iter->second->genomeMap->size();
    }
    return size;
}
