#include "individual.h"

Individual::Individual(unsigned inputNum, unsigned outputNum) {
    this->inputNum = inputNum;
    this->outputNum = outputNum;
    nodeSum = 0;
    innovationNum = 0;
    initialize();
}

Individual::~Individual() {

}

void Individual::initialize() {
    auto sumNode = inputNum + outputNum;
}

void Individual::setInput(float *input) {

}

float *Individual::getOutput() {

}

void Individual::forward() {

}

Individual *Individual::clone() {

}

unsigned Individual::getSize() {

}
