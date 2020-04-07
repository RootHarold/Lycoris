![logo](https://github.com/RootHarold/Lycoris/blob/master/logo/logo.svg)

**Lycoris** is a lightweight and easy-to-use deep learning framework with **neural architecture search**.

Lycoris aims to provide developers with an automated service for end-to-end neural network architecture search, enabling developers to obtain better models with fewer hyperparameter configurations. For more detailed usage of Lycoris, please refer to related [documents](https://github.com/RootHarold/Lycoris#Documents).

At this stage, Lycoris enables you to deploy computation to one or more CPUs in most operating systems. **In the future, GPUs will be added to the minimalist architecture**.

# Features
* Lightweight and portable to smart devices.
* Support online learning.
* Automatic neural network architecture search.
* Support for [Python](https://github.com/RootHarold/Lycoris/tree/master/python). In the near future, other programming languages such as Java, C #, Go, and Rust will be supported.
* Cloud-friendly. Based on C++11, it supports most operating systems and compilers.

# Installation
* C++ version:

```
cd Lycoris
cmake .
sudo make install
```

* Python bindings:

The Python bindings of the code are based on [Pybind11](https://github.com/pybind/pybind11).

```
git clone "https://github.com/pybind/pybind11.git"
cd pybind11
mkdir build
cd build
cmake ..
make install
```

(If **pybind11** and its **header files** are already installed, you can ignore the above steps.)

```
pip install LycorisNet
```

It can also be obtained via manual compilation:

```
cd Lycoris/python
cmake .
make
```

# Documents
The following is the documentation for the C ++ version while that of the Python version can be viewed [here](https://github.com/RootHarold/Lycoris/tree/master/python#Documents).

The APIs provided by **Lycoris** (`namespace LycorisNet`):

Function | Description |  Inputs | Returns
-|-|-|-
**Lycoris**(uint32_t capacity, uint32_t inputDim, uint32_t outputDim, const std::string &mode); | Constructor.<br/> The class Lycoris is the highest level abstraction of LycorisNet. | **capacity**: Capacity of Lycoris.<br/> **inputDim**: Input dimension.<br/> **outputDim**: Output dimension.<br/> **mode**: Mode of Lycoris (classify or predict). | An object of the class Lycoris.
**~Lycoris**(); | Destructor. |  | 
void **preheat**(uint32_t num_of_nodes, uint32_t num_of_connections, uint32_t depth); | Preheating process of the neural network cluster. | **num_of_nodes**: The number of hidden nodes added for each neural network.<br/> **num_of_connections**: The number of connections added for each neural network.<br/> **depth**: Total layers of each neural network. |
void **evolve**(std::vector&#60;std::vector&#60;float&#62; &#62; &input, std::vector&#60;std::vector&#60;float&#62; &#62; &desire); | Evolve the neural network cluster. | **input**: Input data.<br/> **desire**: Expected output data. |
void **fit**(std::vector&#60;std::vector&#60;float&#62; &#62; &input, std::vector&#60;std::vector&#60;float&#62; &#62; &desire); | Fit all neural networks in the neural network cluster. | **input**: Input data.<br/> **desire**: Expected output data. |
void **enrich**(); | Keep only the best one in the neural network cluster. |  |
std::vector&#60;float> **compute**(std::vector&#60;float&#62; &input); | Forward Computing of the best individual. | **input**: Input data. | Returns the output data.
std::vector&#60;std::vector&#60;float&#62; &#62; **computeBatch**(std::vector&#60;std::vector&#60;float&#62; &#62; &input); | Parallel forward Computing of the best individual. | **input**: Input data (two dimensions). | Returns the output data (two dimensions).
void **resize**(uint32_t capacity); | Resize the capacity of the neural network cluster. | **As literally.** |
void **openMemLimit**(uint32_t size); | Turn on memory-limit. | **As literally.** |
void **closeMemLimit**(); | Turn off memory-limit. |  |
void **saveModel**(const std::string &path); | Export the current trained model. | **path**: File path of the current trained model. |
void **setMutateArgs**(std::vector&#60;float&#62; &p); | Set p1 to p4 in the class Args.<br/> Parameters are passed in as `std::vector`. | **p1**: Probability of adding the new node between a connection.<br/> **p2**: Probability of deleting a node.<br/> **p3**: Probability of adding a new connection between two nodes.<br/> **p4**: Probability of deleting a connection. |
void **setMutateOdds**(float odds); | Set the odds of mutating. | The param "odds" means one individual mutates odds times to form odds + 1 individuals. |
void **setCpuCores**(uint32_t num); | Set the number of worker threads to train the model. | **As literally.** |
void **setLR**(float lr); | Set the learning rate. | **As literally.** |
uint32_t **getSize**(); |  |  | Returns the size of the best individual.
uint32_t **getInputDim**(); |  |  | Returns the input dimension.
uint32_t **getOutputDim**(); |  |  | Returns the output dimension.
uint32_t **getCapacity**(); |  |  | Returns capacity of Lycoris.
float **getLoss**(); |  |  | Returns the loss.
std::string **getMode**(); |  |  | Returns mode of Lycoris (classify or predict).
std::vector&#60;uint32_t&#62; **getLayers**(); |  |  | Returns the number of nodes in each layer of the neural network.
std::vector&#60;float&#62; **getHiddenLayer**(uint32_t pos); | The parameter pos starts at index 0. | **pos**: The number of the layer needed. | Returns a vector of nodes in a specific layer of the best individual.
*static* std::string **version**(); |  |  | Returns version information and copyright information.

The funtion used to import the pre-trained model (`namespace LycorisNet`):

Function | Description |  Inputs | Returns
-|-|-|-
Lycoris ***loadModel**(const std::string &path, uint32_t capacity); | Import the pre-trained model. | **path**: File path of the pre-trained model.<br/> **capacity**: Capacity of the neural network cluster. | Returns a pointer to the object of class Lycoris.
Lycoris ***loadViaString**(const std::string &model, uint32_t capacity); | Import the pre-trained model via string. | **model**: The pre-trained model in the form of string.<br/> **capacity**: Capacity of the neural network cluster. | Returns a pointer to the object of class Lycoris.

More detailed documentation will be released in the form of sample code.

# Examples
* [**LycorisAD**](https://github.com/RootHarold/LycorisAD): an elegant outlier detection algorithm framework based on AutoEncoder.
* [**LycorisR**](https://github.com/RootHarold/LycorisR): a lightweight recommendation algorithm framework based on LycorisNet.
* [**LycorisQ**](https://github.com/RootHarold/LycorisQ): a neat reinforcement learning framework based on LycorisNet.
* *More examples will be released in the future.*

# License
Lycoris is released under the [LGPL-3.0](https://github.com/RootHarold/Lycoris/blob/master/LICENSE) license. By using, distributing, or contributing to this project, you agree to the terms and conditions of this license.