#include <cstring>
#include <functional>
#include <pybind11/cast.h>
#include <pybind11/detail/common.h>
#include <pybind11/pytypes.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "pcsx2_interface.h"
#include "pybind11/pybind11.h"

using namespace pybind11::literals;
namespace py = pybind11;

PYBIND11_MODULE(pcsx2_interface, m) {
    m.doc() = "Exposes PS2 memory within a running instance of the PCSX2 emulator using the Pine IPC Protocol";
    
    m.def("is_connected", &PCSX2Interface::is_connected);
    
    m.def("read_bytes", [](uint32_t address, size_t num_of_bytes){
        return py::bytes{reinterpret_cast<const char *>(PCSX2Interface::read_bytes(address, num_of_bytes).data())}; 
    }, "address"_a, "number_of_bytes"_a, "read *num_of_bytes* from ps2 memory starting at *address*");
    
    m.def("read_int8", [](uint32_t address){
        return *reinterpret_cast<const uint8_t *>(PCSX2Interface::read_bytes(address, 1).data());
    }, "address"_a, "read an 8 bit (1 byte) int from ps2 memory at address");
    
    m.def("read_int16", [](uint32_t address){
        return *reinterpret_cast<const uint16_t *>(PCSX2Interface::read_bytes(address, 2).data());
    }, "address"_a, "read a 16 bit (2 byte) int from ps2 memory at address");
    
    m.def("read_int32", [](uint32_t address){
        return *reinterpret_cast<const uint32_t *>(PCSX2Interface::read_bytes(address, 4).data());
    }, "address"_a, "read a 32 bit (4 byte) int from ps2 memory at address");
    
    m.def("read_int64", [](uint32_t address){
        return *reinterpret_cast<const uint64_t *>(PCSX2Interface::read_bytes(address, 8).data());
    }, "address"_a, "read a 64 bit (8 byte) int from ps2 memory at address");
    
    m.def("write_bytes", [](uint32_t address, std::string data){
        PCSX2Interface::write_bytes(address, {data.begin(), data.end()}); 
    });
    m.def("write_int8", [](uint32_t address, uint8_t data){PCSX2Interface::write_int(address, data);});
    m.def("write_int16", [](uint32_t address, uint16_t data){PCSX2Interface::write_int(address, data);});
    m.def("write_int32", [](uint32_t address, uint32_t data){PCSX2Interface::write_int(address, data);});
    m.def("write_int64", [](uint32_t address, uint64_t data){PCSX2Interface::write_int(address, data);});
}