#include <iostream>
#include "individual.h"
#include "args.h"

int main() {
    std::cout << "Hello, the CPP version of Lycoris!" << std::endl;
    auto args = new Args();
    auto *in = new Individual(3, 3, args);
    float *input = new float[3];
    input[0] = 78;
    input[1] = 56;
    input[2] = 34;
    in->setInput(input);
    std::cout << (*(in->nodeMap))[2]->value << std::endl;
    auto output = in->getOutput();
    std::cout << "output: " << output[1] << " " << args->activateFunc(0.52) << std::endl;
    auto son = in->clone();
    son->forward();
    std::cout << son->getSize() << std::endl;
    delete[] output;
    delete in;
    delete args;
    delete[] input;
    delete son;
    return 0;
}
