#include <pybind11/cast.h>
#include <pybind11/detail/common.h>
#include <pybind11/pytypes.h>
#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
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

    m.def("is_connected", &PCSX2Interface::is_connected, "Return True if connected to PCSX2 emulator");
    m.def("connect", &PCSX2Interface::connect, "Attempt to make connection to PCSX2 emulator");
    m.def("disconnect", &PCSX2Interface::disconnect, "Disconnect from PCSX2 emulator");
    m.def("get_game_id", &PCSX2Interface::get_game_id, "Get the Game ID of the currently running game");

    m.def(
        "read_bytes",
        [](uint32_t address, size_t num_of_bytes) {
            auto result = PCSX2Interface::read_bytes(address, num_of_bytes);
            return py::bytes{std::string{result.begin(), result.end()}};
        },
        "address"_a, "number_of_bytes"_a, "read num_of_bytes from ps2 memory starting at address");

    m.def(
        "batch_read",
        [](uint32_t start_address, uint32_t end_address) {
            auto result = PCSX2Interface::batch_read(start_address, end_address);
            return py::bytes{std::string{result.begin(), result.end()}};
        },
        "start_address"_a, "end_address"_a, "quickly read all memory start_address to end_address using PINE batch reads");

    m.def(
        "read_int8", [](uint32_t address) { return PCSX2Interface::read_int<uint8_t>(address); }, "address"_a,
        "read an 8 bit (1 byte) int from ps2 memory at address");

    m.def(
        "read_int16", [](uint32_t address) { return PCSX2Interface::read_int<uint16_t>(address); }, "address"_a,
        "read a 16 bit (2 byte) int from ps2 memory at address");

    m.def(
        "read_int32", [](uint32_t address) { return PCSX2Interface::read_int<uint32_t>(address); }, "address"_a,
        "read a 32 bit (4 byte) int from ps2 memory at address");

    m.def(
        "read_int64", [](uint32_t address) { return PCSX2Interface::read_int<uint64_t>(address); }, "address"_a,
        "read a 64 bit (8 byte) int from ps2 memory at address");

    m.def("write_bytes", [](uint32_t address, std::string data) {
        PCSX2Interface::write_bytes(address, {data.begin(), data.end()});
    }, "address"_a, "data"_a, "write a series of byes to ps2 memory starting at address");
    m.def(
        "write_int8", [](uint32_t address, uint8_t data) { PCSX2Interface::write_int(address, data); }, "address"_a,
        "data"_a, "write a 8 bit (1 byte) int to ps2 memory at address");
    m.def(
        "write_int16", [](uint32_t address, uint16_t data) { PCSX2Interface::write_int(address, data); }, "address"_a,
        "data"_a, "write a 16 bit (2 byte) int to ps2 memory at address");
    m.def(
        "write_int32", [](uint32_t address, uint32_t data) { PCSX2Interface::write_int(address, data); }, "address"_a,
        "data"_a, "write a 32 bit (4 byte) int to ps2 memory at address");
    m.def(
        "write_int64", [](uint32_t address, uint64_t data) { PCSX2Interface::write_int(address, data); }, "address"_a,
        "data"_a, "write a 64 bit (8 byte) int to ps2 memory at address");

    m.def(
        "find_first",
        [](std::string seq, uint32_t start_adr, uint32_t end_adr) {
            auto res = PCSX2Interface::find_first({seq.begin(), seq.end()}, start_adr, end_adr);
            if (!res.has_value()) {
                throw py::value_error("Could not find sequence '" + seq + "' in PS2 memory");
            }
            return res.value();
        },
        "sequence"_a, "start_address"_a = 0x100000, "end_address"_a = PCSX2Interface::PS2_MEMORY_SIZE,
        "return the address of the first occurrence of a sequence of bytes in PS2 memory");
}