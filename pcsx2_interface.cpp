#include "pcsx2_interface.h"

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "pine.h"

using namespace std;
using pcsx2 = PINE::Shared;

namespace PCSX2Interface {

const auto ipc = make_unique<PINE::PCSX2>();

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

unique_ptr<unsigned char> read_8(uint32_t address) {
    if (!is_connected()) {
        throw runtime_error("Read failed: Lost connection to pcsx2");
    }

    try {
        auto result = make_unique<unsigned char>(ipc->Read<uint8_t>(address));
        return result;
    } catch (pcsx2::IPCStatus error) {
        if (error == pcsx2::NoConnection) {
            throw runtime_error("Read failed: Lost connection to pcsx2");
        }
        throw error;
    }
}

} // namespace PCSX2Interface
