#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "pine.h"

namespace PCSX2Interface {

const std::unique_ptr<PINE::PCSX2> ipc = std::make_unique<PINE::PCSX2>();

constexpr size_t PS2_MEMORY_SIZE = 0x2000000;

bool is_connected();
const std::vector<unsigned char> read_bytes(uint32_t address, size_t num_of_bytes);
uint8_t read_int8(uint32_t address);
uint16_t read_int16(uint32_t address);
uint32_t read_int32(uint32_t address);
uint64_t read_int64(uint32_t address);

void write_bytes(uint32_t address, std::vector<unsigned char> data);

template<typename T>
void write_int(uint32_t address, T number) {
    if(!is_connected()) {
        throw std::runtime_error("Lost connection to pcsx2");
    }

    try {
        ipc->Write<T>(address, number);
    } catch (PINE::Shared::IPCStatus error) {
        if (error == PINE::Shared::NoConnection) {
            throw std::runtime_error("Lost connection to pcsx2");
        }
        throw error;
    }
}


} // namespace PCSX2Interface