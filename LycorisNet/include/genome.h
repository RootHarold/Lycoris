#ifndef LYCORIS_GENOME_H
#define LYCORIS_GENOME_H

#include <cstdint>

class Gen {
public:
    uint32_t in;
    uint32_t out;

    Gen();

    Gen(uint32_t in, uint32_t out);

    inline bool operator<(const Gen &gen) const {
        if (in < gen.in) {
            return true;
        } else if (in == gen.in) {
            return out < gen.out;
        } else {
            return false;
        }
    }
};

class Ome {
public:
    float weight;
    bool isEnable;
    uint32_t innovationNum;

    Ome();

    Ome(float weight, bool isEnable, uint32_t innovationNum);
};

#endif //LYCORIS_GENOME_H
