#include <pybind11/cast.h>
#include <pybind11/detail/common.h>
#include <pybind11/pytypes.h>

#include <cstddef>
#include <cstdint>
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
    m.def("is_connected", &PCSX2Interface::is_connected);
    //m.def("read_8", [](uint32_t address){return PCSX2Interface::read_8(address);} );
}