#include "pcsx2_interface.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sys/types.h>

#include "pine.h"

using namespace std;
using pcsx2 = PINE::Shared;

namespace PCSX2Interface {

bool is_connected() {
    try {
        if (ipc->Status() == pcsx2::EmuStatus::Running) {
            return true;
        }
    } catch (pcsx2::IPCStatus error) {
        if (error == pcsx2::NoConnection) {
            return false;
        }
        cout << "Pine Error: " << static_cast<int>(error) << endl;
        throw std::runtime_error("Pine Error");
    }
    return false;
}

const std::vector<unsigned char> read_bytes(uint32_t address, size_t num_of_bytes) {    
    if(address + num_of_bytes >= PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to read outside PS2 memory range");
    }

    if(!is_connected()) {
        throw runtime_error("Lost connection to pcsx2");
    }

    try {
        // TODO: should utilize pine batch reads for larger read requests
        std::vector<unsigned char> result;
        for(int i = 0; i < num_of_bytes; ++i) {
            result.push_back(ipc->Read<uint8_t>(address + i));
        }
        return result;
    } catch (pcsx2::IPCStatus error) {
        if (error == pcsx2::NoConnection) {
            throw runtime_error("Lost connection to pcsx2");
        }
        throw error;
    }
}

void write_bytes(uint32_t address, std::vector<unsigned char> data) {
    return;
}


} // namespace PCSX2Interface
