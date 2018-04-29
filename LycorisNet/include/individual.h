#ifndef LYCORIS_INDIVIDUAL_H
#define LYCORIS_INDIVIDUAL_H

#include <vector>
#include <map>
#include "node.h"
#include "args.h"

class Individual {
public:
    unsigned inputNum;
    unsigned outputNum;
    unsigned innovationNum;
    unsigned nodeSum;
    float fitness;
    std::vector<int> *nodeSlice;
    std::map<int, Node *> *nodeMap;
    Args *args;

    Individual(unsigned inputNum, unsigned outputNum);

    ~Individual();

    void initialize();

    void setInput(float *input);

    float *getOutput();

    void forward();

    Individual *clone();

    unsigned getSize();
};

#endif //LYCORIS_INDIVIDUAL_H
