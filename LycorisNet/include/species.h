#ifndef LYCORIS_SPECIES_H
#define LYCORIS_SPECIES_H

#include <vector>
#include "individual.h"

class Species {
public:
    std::vector<Individual *> *individualList;

    Species();

    ~Species();
};

#endif //LYCORIS_SPECIES_H
