#include "lycoris.h"

Lycoris::Lycoris() {

}

Lycoris::Lycoris(unsigned capacity) {
    this->capacity = capacity;
    individualList = new Individual *[capacity];
}

Lycoris::~Lycoris() {
    for (unsigned i = 0; i < capacity; ++i) {
        delete individualList[i];
    }
    delete[] individualList;
}

std::string Lycoris::version() {
    return "Lycoris core 1.8-dev-6";
}
