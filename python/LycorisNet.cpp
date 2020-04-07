#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lycoris.h"

namespace py = pybind11;

PYBIND11_MODULE(LycorisNet, m) {
    m.doc() = "A lightweight and easy-to-use deep learning framework with neural architecture search.";

    py::class_<LycorisNet::Lycoris>(m, "Lycoris")
            .def(py::init<uint32_t, uint32_t, uint32_t, const std::string &>(), py::arg("capacity"),
                 py::arg("inputDim"), py::arg("outputDim"), py::arg("mode"))
            .def("preheat", &LycorisNet::Lycoris::preheat, py::arg("nodes"), py::arg("connections"), py::arg("depths"),
                 "Preheating process of the neural network cluster.")
            .def("evolve", &LycorisNet::Lycoris::evolve, py::arg("input"), py::arg("desire"),
                 "Evolve the neural network cluster.")
            .def("fit", &LycorisNet::Lycoris::fit, py::arg("input"), py::arg("desire"),
                 "Fit all neural networks in the neural network cluster.")
            .def("enrich", &LycorisNet::Lycoris::enrich, "Keep only the best one in the neural network cluster.")
            .def("compute", &LycorisNet::Lycoris::compute, py::arg("input"),
                 "Forward Computing of the best individual.")
            .def("computeBatch", &LycorisNet::Lycoris::computeBatch, py::arg("input"),
                 "Parallel forward Computing of the best individual.")
            .def("resize", &LycorisNet::Lycoris::resize, py::arg("capacity"),
                 "Resize the capacity of the neural network cluster.")
            .def("openMemLimit", &LycorisNet::Lycoris::openMemLimit, py::arg("size"), "Turn on memory-limit.")
            .def("closeMemLimit", &LycorisNet::Lycoris::closeMemLimit, "Turn off memory-limit.")
            .def("saveModel", &LycorisNet::Lycoris::saveModel, py::arg("path"), "Export the current trained model.")
            .def("setMutateArgs", &LycorisNet::Lycoris::setMutateArgs, py::arg("p"), "Set p1 to p4 in the class Args.")
            .def("setMutateOdds", &LycorisNet::Lycoris::setMutateOdds, py::arg("odds"), "Set the odds of mutating.")
            .def("setCpuCores", &LycorisNet::Lycoris::setCpuCores, py::arg("num"),
                 "Set the number of worker threads to train the model.")
            .def("setLR", &LycorisNet::Lycoris::setLR, py::arg("lr"), "Set the learning rate.")
            .def("getSize", &LycorisNet::Lycoris::getSize, "Returns the size of the best individual.")
            .def("getInputDim", &LycorisNet::Lycoris::getInputDim, "Returns the input dimension.")
            .def("getOutputDim", &LycorisNet::Lycoris::getOutputDim, "Returns the output dimension.")
            .def("getCapacity", &LycorisNet::Lycoris::getCapacity, "Returns capacity of Lycoris.")
            .def("getLoss", &LycorisNet::Lycoris::getLoss, "Returns the loss.")
            .def("getMode", &LycorisNet::Lycoris::getMode, "Returns mode of Lycoris (classify or predict).")
            .def("getLayers", &LycorisNet::Lycoris::getLayers,
                 "Returns the number of nodes in each layer of the neural network.")
            .def("getHiddenLayer", &LycorisNet::Lycoris::getHiddenLayer, py::arg("pos"),
                 "Get a vector of nodes in a specific layer of the best individual.")
            .def("version", &LycorisNet::Lycoris::version, "Returns version information and copyright information.");

    m.def("loadModel", &LycorisNet::loadModel, py::arg("path"), py::arg("capacity"),
          "Import the pre-trained model.");
    m.def("loadViaString", &LycorisNet::loadViaString, py::arg("model"), py::arg("capacity"),
          "Import the pre-trained model via string.");
}