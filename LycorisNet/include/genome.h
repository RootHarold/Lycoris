/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_GENOME_H
#define LYCORIS_GENOME_H

#include <cstdint>

/*
    The class "Gen" and the class "Ome" form connectionsc between nodes.
    The former contains the numbers of input and output.
    The latter consists of weight, usability and the cumulative number of
    connections ("innovationNum").
 */

class Gen {
public:
    uint32_t in;
    uint32_t out;

    Gen();

    Gen(uint32_t in, uint32_t out);

    // Overloading operator.
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
