/*
    Copyright (c) 2018, RootHarold
    All rights reserved.

    Use of this source code is governed by an Apache license that can be found
    in the LICENSE file.
*/

#include <iostream>
#include "lycoris.h"

using namespace LycorisNet;

void handle(Individual &in) {
    float input[1] = {0.5};
    in.setInput(input);
    in.forward();
    float result[10];
    in.getOutput(result);
    float desire[10] = {0.5625, 0.7890625, 0.84375, 0.84375, 0.8671875, 0.6796875, 0.8671875, 0.890625, 0.84375,
                        0.78125};
    in.fitness = 0 - LycorisUtils::euclidean_distance(result, desire, 10);
}

int main() {
    std::cout << "Powered by: " << LycorisUtils::version() << std::endl;
    Lycoris lie(2400, 1, 10);
    lie.setForwardFunc(handle);
    lie.openMemLimit(256);

    for (int i = 1; i <= 300; ++i) {
        lie.runLycoris();
        if (i % 60 == 0) {
            std::cout << "result: " << lie.best->fitness * 10000 << std::endl;
        }
    }

    float result[10];
    lie.best->getOutput(result);

    char ch[12];
    for (int j = 0; j < 11; ++j) {
        if (j < 5) {
            ch[j] = int(round(result[j] * 128));
        } else if (j > 5) {
            ch[j] = int(round(result[j - 1] * 128));
        } else {
            ch[j] = ' ';
        }
    }
    ch[11] = '\0';

    std::cout << ch << "!" << std::endl;

    lie.exportLycoris("HelloWorld");

    return 0;
}
