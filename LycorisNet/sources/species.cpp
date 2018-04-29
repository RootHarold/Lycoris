#include "species.h"

Species::Species() {

}

Species::~Species() {
    for (std::vector<Individual *>::iterator iter = individualList->begin(); iter != individualList->end(); ++iter) {
        delete *iter;
    }

    delete individualList;
}
