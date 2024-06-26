#include "pcsx2_interface.h"

#include <array>
#include <cmath>
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
    if(address + num_of_bytes > PS2_MEMORY_SIZE) {
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
    if(address + data.size() > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to write outside PS2 memory range");
    }

    if(!is_connected()) {
        throw runtime_error("Lost connection to pcsx2");
    }

    try {
        // TODO: should utilize pine batch write for larger write requests
        std::vector<unsigned char> result;
        for(int i = 0; i < data.size(); ++i) {
            ipc->Write<uint8_t>(address + i, data[i]);
        }
    } catch (pcsx2::IPCStatus error) {
        if (error == pcsx2::NoConnection) {
            throw runtime_error("Lost connection to pcsx2");
        }
        throw error;
    }
}

uint32_t find_first(const std::vector<unsigned char> seq, uint32_t start, uint32_t end) {
    if(start >= PS2_MEMORY_SIZE || end > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to read outside PS2 memory range");
    }

    int byte_count = end - start;
    if(byte_count <= 0) {
        throw std::length_error("Non-positive range");
    }
    
    const size_t BYTES_PER_BATCH = MAX_BATCH_REPLY_COUNT * 8;
    int batch_count = byte_count / (BYTES_PER_BATCH);
    int bytes_remaining = byte_count / (BYTES_PER_BATCH);


    for(auto batch_idx : {batch_count}) {
        ipc->InitializeBatch();
        for(auto j : {MAX_BATCH_REPLY_COUNT}) {
            ipc->Read<uint64_t, true>(start + (batch_idx * BYTES_PER_BATCH + j * 8));
        }
        auto resr = ipc->FinalizeBatch();
        ipc->SendCommand(resr);

        for(auto byte_idx : {BYTES_PER_BATCH}) {
            if(ipc->GetReply<pcsx2::MsgRead8>(resr, byte_idx) == seq[0]) {
                vector<unsigned char> buffer;
                for(const auto i : {seq.size()}) {
                    buffer.push_back(ipc->GetReply<pcsx2::MsgRead8>(resr, byte_idx + i));
                }

                if(buffer == seq) {
                    return start + batch_idx * BYTES_PER_BATCH + byte_idx;
                }
            }
        }
    }

    return 0;
}

} // namespace PCSX2Interface
