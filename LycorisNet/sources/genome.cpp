#include "genome.h"

Gen::Gen() {

}

Gen::Gen(uint32_t in, uint32_t out) {
    this->in = in;
    this->out = out;
}

Ome::Ome() {

}

Ome::Ome(float weight, bool isEnable, uint32_t innovationNum) {
    this->weight = weight;
    this->isEnable = isEnable;
    this->innovationNum = innovationNum;
}
