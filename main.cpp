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
    auto lie1 = new Lycoris(256, 2, 2);
    lie1->setForwardFunc(handle);
    for (int i = 0; i < 4; ++i) {
        lie1->runLycoris();
        std::cout << lie1->best->fitness << std::endl;
    }
    lie1->exportLycoris("model");
    auto lie2 = Lycoris::importLycoris("model", 256);
    lie2->setForwardFunc(handle);
    std::cout << lie2->best->fitness << std::endl;
    for (int i = 0; i < 16; ++i) {
        lie2->runLycoris();
        std::cout << lie2->best->fitness << std::endl;
    }
    delete lie2;
    delete lie1;
    return 0;
}
