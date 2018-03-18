#ifndef LYCORIS_GENOME_H
#define LYCORIS_GENOME_H

#include <cstring>
#include <string>
#include <sstream>

class Gen {
public:
    unsigned in;
    unsigned out;

    Gen();

    Gen(unsigned in, unsigned out);

    Gen clone();

    bool operator<(const Gen &gen) const {
        std::stringstream stream1;
        stream1 << in << out;
        std::stringstream stream2;
        stream2 << gen.in << gen.out;
        return stream1.str() < stream2.str();
    }
};

class Ome {
public:
    float weight;
    bool isEnable;
    unsigned innovationNum;

    Ome();

    Ome(float weight, bool isEnable, unsigned innovationNum);

    Ome clone();
};

#endif //LYCORIS_GENOME_H
