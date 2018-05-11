#include "genome.h"

Gen::Gen() {

}

Gen::Gen(uint32_t in, uint32_t out) {
    this->in = in;
    this->out = out;
}

Gen Gen::clone() {
    return Gen(in, out);
}

Ome::Ome() {

}

Ome::Ome(float weight, bool isEnable, uint32_t innovationNum) {
    this->weight = weight;
    this->isEnable = isEnable;
    this->innovationNum = innovationNum;
}

Ome Ome::clone() {
    return Ome(weight, isEnable, innovationNum);
}
