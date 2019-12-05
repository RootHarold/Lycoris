#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lycoris.h"

namespace py = pybind11;

PYBIND11_MODULE(LycorisNet, m) {
    // m.doc() = "Description...";

    py::class_<LycorisNet::Lycoris>(m, "Lycoris")
            .def(py::init<uint32_t, uint32_t, uint32_t, const std::string &>())
            .def("preheat", &LycorisNet::Lycoris::preheat)
            .def("evolve", &LycorisNet::Lycoris::evovle_)
            .def("enrich", &LycorisNet::Lycoris::enrich)
            .def("compute", &LycorisNet::Lycoris::compute)
            .def("resize", &LycorisNet::Lycoris::resize)
            .def("openMemLimit", &LycorisNet::Lycoris::openMemLimit)
            .def("closeMemLimit", &LycorisNet::Lycoris::closeMemLimit)
            .def("version", &LycorisNet::Lycoris::version);
}