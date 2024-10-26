#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "pine.h"

namespace PCSX2Interface {

inline std::unique_ptr<PINE::PCSX2> ipc = nullptr;

constexpr size_t PS2_MEMORY_SIZE = 0x2000000;
constexpr size_t BYTES_PER_BATCH = 102400;

bool is_connected();
void connect();
void disconnect();
std::string get_game_id();
const std::vector<unsigned char> read_bytes(uint32_t address, size_t num_of_bytes);
const std::vector<unsigned char> batch_read(uint32_t start_adr, uint32_t end_adr);
void write_bytes(uint32_t address, std::vector<unsigned char> data);
std::optional<uint32_t> find_first(const std::vector<unsigned char> seq, uint32_t start = 0x0, uint32_t end = PS2_MEMORY_SIZE);

template<typename T>
T read_int(uint32_t address) {
    if(address + sizeof(T) > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to read outside PS2 memory range");
    }

    if(!is_connected()) {
        throw std::runtime_error("Read Failed. No connection to pcsx2");
    }

    try {
        return ipc->Read<T>(address);
    } catch (PINE::Shared::IPCStatus error) {
        if (error == PINE::Shared::NoConnection) {
            throw std::runtime_error("Read Failed. No connection to pcsx2");
        }
        throw error;
    }
}

template<typename T>
void write_int(uint32_t address, T number) {
    if(address + sizeof(T) > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to read outside PS2 memory range");
    }

    if(!is_connected()) {
        throw std::runtime_error("Write Failed. No connection to pcsx2");
    }

    try {
        ipc->Write<T>(address, number);
    } catch (PINE::Shared::IPCStatus error) {
        if (error == PINE::Shared::NoConnection) {
            throw std::runtime_error("Write Failed. No connection to pcsx2");
        }
        throw error;
    }
}


} // namespace PCSX2Interface