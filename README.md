# Lycoris
**Lycoris** is a lightweight neural architecture search framework based on *Evolving Neural Networks through Augmenting Topologies*.

Lycoris aims to provide developers with an automated service for end-to-end neural network architecture search, enabling developers to obtain better models with fewer hyperparameter configurations. For more detailed usage of Lycoris, please refer to related [documents](https://github.com/RootHarold/Lycoris#Documents).

At this stage, Lycoris enables you to deploy computation to one or more CPUs in most operating systems. **In the future, GPUs will be added to the minimalist architecture**.

# Features
* Lightweight and portable to smart devices.
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

```
cd Lycoris/python
cmake .
make
```

The Python bindings of the code are based on [Pybind11](https://github.com/pybind/pybind11).

# Documents
**Lycoris 2 will be released soon**.

# License
Lycoris is released under the [LGPL-3.0](https://github.com/RootHarold/Lycoris/blob/master/LICENSE) license. By using, distributing, or contributing to this project, you agree to the terms and conditions of this license.