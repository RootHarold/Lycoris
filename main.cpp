#include <iostream>
#include "lycoris.h"

int main() {
    std::cout << "Hello, the CPP version of Lycoris!" << std::endl;
    Args *args = new Args();
    Individual *in1 = new Individual(3, 3, args);
    Individual *in2 = new Individual(3, 3, args);
    std::cout << distance(*in1, *in2) << std::endl;
    auto son = mateIndividual(*in1, *in2);
    delete son;
    delete in1;
    delete in2;
    delete args;
    return 0;
}
