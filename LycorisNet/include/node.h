#ifndef LYCORIS_NODE_H
#define LYCORIS_NODE_H

#include <map>
#include "genome.h"

class Node {
public:
    uint32_t nodeNum;
    uint32_t nodeType;
    float value;
    float bias;
    std::map<Gen, Ome> *genomeMap;

    Node(uint32_t nodeNum, uint32_t nodeType);

    ~Node();

    void initializeBias(float f);

    Node *clone();
};

#endif //LYCORIS_NODE_H
