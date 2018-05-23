#include <iostream>
#include "lycoris.h"

void handle(Individual &in) {
    auto input = new float[2];
    auto output = new float[2];
    input[0] = 0;
    input[1] = 1;
    output[0] = 0.5;
    output[1] = 0.5;

    in.setInput(input);
    in.forward();
    auto out = in.getOutput();
    float fitness = 0;
    for (int i = 0; i < 2; ++i) {
        fitness -= (output[i] - out[i]) * (output[i] - out[i]);
    }
    in.fitness = fitness;

    delete[] input;
    delete[] output;
    delete[] out;
}

int main() {
    std::cout << "Hello, the CPP version of Lycoris!" << std::endl;
    auto lie = new Lycoris(256, 2, 2);
    lie->setForwardFunc(handle);
    for (int i = 0; i < 16; ++i) {
        lie->runLycoris();
        std::cout << lie->best->fitness << std::endl;
    }
    delete lie;
    return 0;
}
