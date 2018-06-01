#ifndef LYCORIS_INDIVIDUAL_H
#define LYCORIS_INDIVIDUAL_H

#include <vector>
#include <map>
#include "node.h"
#include "args.h"

class Individual {
public:
    uint32_t inputNum;
    uint32_t outputNum;
    uint32_t innovationNum;
    uint32_t nodeSum;
    float fitness;
    std::vector<uint32_t> *nodeSlice;
    std::map<uint32_t, Node *> *nodeMap;
    Args *args;

    Individual();

    Individual(uint32_t inputNum, uint32_t outputNum, Args *args);

    ~Individual();

    void initialize();

    void setInput(float *input);

    float *getOutput();

    void forward();

    Individual *clone();

    uint32_t getSize();

    static Individual* importIndividual(std::string path);

    void exportIndividual(std::string path);
};

#endif //LYCORIS_INDIVIDUAL_H
