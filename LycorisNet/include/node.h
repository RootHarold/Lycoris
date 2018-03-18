#ifndef LYCORIS_NODE_H
#define LYCORIS_NODE_H

#include <map>
#include "genome.h"

class Node {
public:
    unsigned nodeNum;
    unsigned nodeType;
    float value;
    float bias;
    std::map<Gen, Ome> *genomeMap;

    Node(unsigned nodeNum, unsigned nodeType);

    ~Node();

    Node  *clone();
};

#endif //LYCORIS_NODE_H
