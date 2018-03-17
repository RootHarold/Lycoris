#ifndef LYCORIS_GENOME_H
#define LYCORIS_GENOME_H

class Gen {
public:
    unsigned in;
    unsigned out;

    Gen();

    Gen(unsigned in, unsigned out);
};

class Ome {
public:
    float weight;
    bool isEnable;
    unsigned innovationNum;

    Ome();

    Ome(float weight, bool isEnable, unsigned innovationNum);
};

#endif //LYCORIS_GENOME_H
