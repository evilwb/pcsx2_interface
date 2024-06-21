#include <pybind11/detail/common.h>

#include <iostream>
#include <memory>
#include <string>

#include "pcsx2_interface.h"
#include "pybind11/pybind11.h"
namespace py = pybind11;

void echo(const std::string string) {
    std::cout << string << std::endl;
}

PYBIND11_MODULE(pcsx2_interface, m) {
    m.doc() = "Exposes PS2 memory within a running instance of the PCSX2 emulator using the Pine IPC Protocol";
    py::class_<PCSX2Interface, std::unique_ptr<PCSX2Interface, py::nodelete>>(m, "PCSX2Interface",
                                                                              "This is a singleton class")
        .def(py::init([]() { return std::unique_ptr<PCSX2Interface, py::nodelete>(&PCSX2Interface::get_instance()); }),
             "This class is created by getting a reference to a statically allocated instance of the singleton")
        .def("is_connected", &PCSX2Interface::is_connected);
}