/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_LYCORIS_H
#define LYCORIS_LYCORIS_H

#include <vector>
#include <string>
#include "utils.h"

namespace LycorisNet {

    /*
     * The class Lycoris is the highest level abstraction of LycorisNet.
     */

    class Lycoris {
    public:

        /*
         * Constructor.
         *
         * capacity: Capacity of Lycoris.
         *
         * inputDim: Input dimension.
         *
         * outputDim: Output dimension.
         *
         * mode: Mode of Lycoris (classify or predict).
         */

        Lycoris(uint32_t capacity, uint32_t inputDim, uint32_t outputDim, const std::string &mode);

        /*
         * Destructor.
         */

        ~Lycoris();

        /*
         * Preheating process of the neural network cluster.
         *
         * num_of_nodes: The number of hidden nodes added for each neural network.
         *
         * num_of_connections: The number of connections added for each neural network.
         *
         * depth: Total layers of each neural network.
         */

        void preheat(uint32_t num_of_nodes, uint32_t num_of_connections, uint32_t depth);

        /*
         * Evolve the neural network cluster.
         *
         * input: Input data.
         *
         * desire: Expected output data.
         */

        void evolve(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire);

        /*
         * Fit all neural networks in the neural network cluster.
         *
         * input: Input data.
         *
         * desire: Expected output data.
         */

        void fit(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire);

        /*
         * Keep only the best one in the neural network cluster.
         */

        void enrich();

        /*
         * Forward Computing of the best individual.
         *
         * input: Input data.
         *
         * Returns the output data.
         */

        std::vector<float> compute(std::vector<float> &input);

        /*
         * Parallel forward Computing of the best individual.
         *
         * input: Input data (two dimensions).
         *
         * Returns the output data (two dimensions).
         */

        std::vector<std::vector<float> > computeBatch(std::vector<std::vector<float> > &input);

        /*
         * Resize the capacity of the neural network cluster.
         */

        void resize(uint32_t capacity);

        /*
         * Turn on memory-limit.
         */

        void openMemLimit(uint32_t size);

        /*
         * Turn off memory-limit.
         */

        void closeMemLimit();

        /*
         * Export the current trained model.
         *
         * path: File path of the current trained model.
         */

        void saveModel(const std::string &path);

        /*
         * Set p1 to p4 in the class Args.
         *
         * p1: Probability of adding the new node between a connection.
         *
         * p2: Probability of deleting a node.
         *
         * p3: Probability of adding a new connection between two nodes.
         *
         * p4: Probability of deleting a connection.
         */

        void setMutateArgs(std::vector<float> &p);

        /*
         * Set the odds of mutating.
         *
         * The param "odds" means one individual mutates odds times
         * to form odds + 1 individuals.
         */

        void setMutateOdds(float odds);

        /*
         * Set the number of worker threads to train the model.
         */

        void setCpuCores(uint32_t num);

        /*
         * Set the learning rate.
         */

        void setLR(float lr);

        /*
         * Returns the size of the best individual.
         */

        uint32_t getSize();

        /*
         * Returns the input dimension.
         */

        uint32_t getInputDim();

        /*
         * Returns the output dimension.
         */

        uint32_t getOutputDim();

        /*
         * Returns capacity of Lycoris.
         */

        uint32_t getCapacity();

        /*
         * Returns the loss.
         */

        float getLoss();

        /*
         * Returns mode of Lycoris (classify or predict).
         */

        std::string getMode();

        /*
         * Returns the number of nodes in each layer of the neural network.
         */
        std::vector<uint32_t> getLayers();

        /*
         * Get a vector of nodes in a specific layer of the best individual.
         *
         * pos: The number of the layer needed and it starts at index 0.
         */
        std::vector<float> getHiddenLayer(uint32_t pos);

        /*
         * Returns version information and copyright information.
         */

        inline static std::string version() {
            return "LycorisNet Core 2.10.36 (CPU Mode) By RootHarold.";
        }

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

        friend Lycoris *loadViaString(const std::string &model, uint32_t capacity);

    private:
        // The dimension of input.
        uint32_t inputNum;
        // The dimension of output.
        uint32_t outputNum;
        // The capacity of Lycoris.
        uint32_t capacity;
        // The best individual.
        Individual *best;

        // All neural networks share an object of the class Args.
        Args *args;

        // Store the individuals in the neural network cluster.
        std::vector<Individual *> *individualList;

        // Assist mutation operation.
        std::vector<Individual *> tempList;

        // Store the length of individualList.
        uint32_t oldLength;

        // Used in computeBatch().
        std::vector<std::vector<float> > *data_for_computeBatch;

        // Fit all neural networks in the neural network cluster.
        void fitAll(std::vector<std::vector<float> > &input, std::vector<std::vector<float> > &desire);

        // Mutating.
        void mutate();

        // The functor of mutate().
        void mutateCore(uint32_t start, uint32_t end);

        /*
         * BP_Single_Thread() is invoked in every individual of the
         * neural network cluster.
         */

        void backPropagation();

        // The functor of backPropagation().
        void backPropagationCore(uint32_t start, uint32_t end);

        /*
         * All the individuals in the neural network cluster are sorted
         * and the better ones are selected.
         */

        void chooseElite();

        // Main part of evolve().
        void runLycoris();

        // The functor of preheat().
        void preheatCore(uint32_t start, uint32_t end, uint32_t num_of_nodes, uint32_t num_of_connections);

        // The functor of computeBatch().
        void computeBatchCore(uint32_t start, uint32_t end, float **output);

        // Check if the individualList is initialized.
        void checkFirstRun();
    };

    /*
     * Import the pre-trained model.
     *
     * path: File path of the pre-trained model.
     *
     * capacity: Capacity of the neural network cluster.
     *
     * Returns a pointer to the object of class Lycoris.
     */

    Lycoris *loadModel(const std::string &path, uint32_t capacity);

    /*
     * Import the pre-trained model via string.
     *
     * model: The pre-trained model in the form of string.
     *
     * capacity: Capacity of the neural network cluster.
     *
     * Returns a pointer to the object of class Lycoris.
     */

    Lycoris *loadViaString(const std::string &model, uint32_t capacity);

}

#endif //LYCORIS_LYCORIS_H
