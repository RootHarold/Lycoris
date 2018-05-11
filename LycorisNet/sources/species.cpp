#include "species.h"

Species::Species() {

}

Species::~Species() {
    for (auto iter = individualList->begin(); iter != individualList->end(); ++iter) {
        delete *iter;
    }

    delete individualList;
}
