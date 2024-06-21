#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "pine.h"

namespace PCSX2Interface {

bool is_connected();
std::unique_ptr<unsigned char> read_8(uint32_t address);
void write_8(uint32_t address, std::byte data);


} // namespace PCSX2Interface