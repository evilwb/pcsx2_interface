#include "pcsx2_interface.h"

#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "pine.h"

using namespace std;
using pcsx2 = PINE::Shared;

PCSX2Interface& PCSX2Interface::get_instance() {
    static PCSX2Interface instance;
    return instance;
}

PCSX2Interface::PCSX2Interface() : ipc(std::make_unique<PINE::PCSX2>()) {}

bool PCSX2Interface::is_connected() {
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
