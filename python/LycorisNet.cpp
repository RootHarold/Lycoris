#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lycoris.h"

namespace py = pybind11;

PYBIND11_MODULE(LycorisNet, m) {
    m.doc() = "A lightweight neural architecture search framework based on evolutionary strategies.";

    py::class_<LycorisNet::Lycoris>(m, "Lycoris")
            .def(py::init<uint32_t, uint32_t, uint32_t, const std::string &>(), py::arg("capacity"),
                 py::arg("inputDim"), py::arg("outputDim"), py::arg("mode"))
            .def("preheat", &LycorisNet::Lycoris::preheat, py::arg("nodes"), py::arg("connections"), py::arg("depths"))
            .def("evolve", &LycorisNet::Lycoris::evolve, py::arg("input"), py::arg("desire"))
            .def("fit", &LycorisNet::Lycoris::fit, py::arg("input"), py::arg("desire"))
            .def("fitAll", &LycorisNet::Lycoris::fitAll, py::arg("input"), py::arg("desire"))
            .def("enrich", &LycorisNet::Lycoris::enrich)
            .def("compute", &LycorisNet::Lycoris::compute, py::arg("input"))
            .def("resize", &LycorisNet::Lycoris::resize, py::arg("capacity"))
            .def("openMemLimit", &LycorisNet::Lycoris::openMemLimit, py::arg("size"))
            .def("closeMemLimit", &LycorisNet::Lycoris::closeMemLimit)
            .def("saveModel", &LycorisNet::Lycoris::saveModel, py::arg("path"))
            .def("setMutateArgs", &LycorisNet::Lycoris::setMutateArgs, py::arg("p"))
            .def("setMutateOdds", &LycorisNet::Lycoris::setMutateOdds, py::arg("odds"))
            .def("setCpuCores", &LycorisNet::Lycoris::setCpuCores, py::arg("num"))
            .def("setLR", &LycorisNet::Lycoris::setLR, py::arg("lr"))
            .def("getSize", &LycorisNet::Lycoris::getSize)
            .def("getInputDim", &LycorisNet::Lycoris::getInputDim)
            .def("getOutputDim", &LycorisNet::Lycoris::getOutputDim)
            .def("getCapacity", &LycorisNet::Lycoris::getCapacity)
            .def("getFitness", &LycorisNet::Lycoris::getFitness)
            .def("getMode", &LycorisNet::Lycoris::getMode)
            .def("version", &LycorisNet::Lycoris::version);

    m.def("loadModel", &LycorisNet::loadModel, py::arg("path"), py::arg("capacity"),
          "Import the pre-trained model.");
}