/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_SPECIES_H
#define LYCORIS_SPECIES_H

#include <vector>
#include "individual.h"

/*
    This class just contains a individual slice.
 */

class Species {
public:
    std::vector<Individual *> *individualList;

    Species();

    ~Species();
};

#endif //LYCORIS_SPECIES_H
