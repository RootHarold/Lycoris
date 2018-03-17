#include "genome.h"

Gen::Gen() {

}

Gen::Gen(unsigned in, unsigned out) {
    this->in = in;
    this->out = out;
}

Ome::Ome() {

}

Ome::Ome(float weight, bool isEnable, unsigned innovationNum) {
    this->weight = weight;
    this->isEnable = isEnable;
    this->innovationNum = innovationNum;
}
