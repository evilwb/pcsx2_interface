#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "pine.h"

class PCSX2Interface {
   public:
    PCSX2Interface(PCSX2Interface const&) = delete;
    void operator=(PCSX2Interface const&) = delete;
    static PCSX2Interface& get_instance();

    bool is_connected();
    std::vector<std::byte> read_address(uint32_t address, size_t bytes_to_read);
    void write_address(uint32_t address, std::vector<std::byte> data);

   private:
    PCSX2Interface();
    std::unique_ptr<PINE::PCSX2> ipc;
};