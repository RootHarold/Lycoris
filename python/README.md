![logo](https://github.com/RootHarold/Lycoris/blob/master/logo/logo.svg)

LycorisNet's Python bindings are implemented based on [Pybind11](https://github.com/pybind/pybind11).
# Installation
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
The APIs provided by **Lycoris** (`from LycorisNet import Lycoris`):

Function | Description |  Inputs | Returns
-|-|-|-
**Lycoris**(capacity, inputDim, outputDim, mode) | Constructor.<br/> The class Lycoris is the highest level abstraction of LycorisNet. | **capacity**: Capacity of Lycoris.<br/> **inputDim**: Input dimension.<br/> **outputDim**: Output dimension.<br/> **mode**: Mode of Lycoris (classify or predict). | An object of the class Lycoris.
**preheat**(nodes, connections, depths) | Preheating process of the neural network cluster. | **nodes**: The number of hidden nodes added for each neural network.<br/> **connections**: The number of connections added for each neural network.<br/> **depths**: Total layers of each neural network. |
**evolve**(input, desire) | Evolve the neural network cluster. | **input**: Input data.<br/> **desire**: Expected output data. |
**fit**(input, desire) | Fit all neural networks in the neural network cluster. | **input**: Input data.<br/> **desire**: Expected output data. |
**enrich**() | Keep only the best one in the neural network cluster. |  |
**compute**(input) | Forward Computing of the best individual. | **input**: Input data. | Returns the output data.
**computeBatch**(input) | Parallel forward Computing of the best individual. | **input**: Input data (two dimensions). | Returns the output data (two dimensions).
**resize**(capacity) | Resize the capacity of the neural network cluster. | **As literally.** |
**openMemLimit**(size) | Turn on memory-limit. | **As literally.** |
**closeMemLimit**() | Turn off memory-limit. |  |
**saveModel**(path) | Export the current trained model. | **path**: File path of the current trained model. |
**setMutateArgs**(p) | Set p1 to p4 in the class Args.<br/> Parameters are passed in as `List`. | **p1**: Probability of adding the new node between a connection.<br/> **p2**: Probability of deleting a node.<br/> **p3**: Probability of adding a new connection between two nodes.<br/> **p4**: Probability of deleting a connection. |
**setMutateOdds**(odds) | Set the odds of mutating. | The param "odds" means one individual mutates odds times to form odds + 1 individuals. |
**setCpuCores**(num) | Set the number of worker threads to train the model. | **As literally.** |
**setLR**(lr) | Set the learning rate. | **As literally.** |
**getSize**() |  |  | Returns the size of the best individual.
**getInputDim**() |  |  | Returns the input dimension.
**getOutputDim**() |  |  | Returns the output dimension.
**getCapacity**() |  |  | Returns capacity of Lycoris.
**getLoss**() |  |  | Returns the loss.
**getMode**() |  |  | Returns mode of Lycoris (classify or predict).
**getLayers**() |  |  | Returns the number of nodes in each layer of the neural network.
**getHiddenLayer**(pos) | The parameter pos starts at index 0. | **pos**: The number of the layer needed. | Returns a vector of nodes in a specific layer of the best individual.
`@staticmethod`<br/>**version**() |  |  | Returns version information and copyright information.

The funtion used to import the pre-trained model (`from LycorisNet import loadModel, loadViaString`):

Function | Description |  Inputs | Returns
-|-|-|-
Lycoris **loadModel**(path, capacity) | Import the pre-trained model. | **path**: File path of the pre-trained model.<br/> **capacity**: Capacity of the neural network cluster. | Returns an object of class Lycoris.
Lycoris **loadViaString**(model, capacity) | Import the pre-trained model via string. | **model**: The pre-trained model in the form of string.<br/> **capacity**: Capacity of the neural network cluster. | Returns an object of class Lycoris.

Information related to parameters and return values also appears within:

```
>>> help(Lycoris)
>>> help(loadModel)
```

# Examples
* [**LycorisAD**](https://github.com/RootHarold/LycorisAD): an elegant outlier detection algorithm framework based on AutoEncoder.
* [**LycorisR**](https://github.com/RootHarold/LycorisR): a lightweight recommendation algorithm framework based on LycorisNet.
* [**LycorisQ**](https://github.com/RootHarold/LycorisQ): a neat reinforcement learning framework based on LycorisNet.
* *More examples will be released in the future.*

# License
Lycoris is released under the [LGPL-3.0](https://github.com/RootHarold/Lycoris/blob/master/LICENSE) license. By using, distributing, or contributing to this project, you agree to the terms and conditions of this license.