#include "pcsx2_interface.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <sys/types.h>

#include "pine.h"

using namespace std;
using pcsx2 = PINE::Shared;

namespace PCSX2Interface {

bool is_connected() {
    if(!ipc) {
        return false;
    }
    
    try {
        if(ipc->Status() == pcsx2::EmuStatus::Running) {
            return true;
        }
    } catch (pcsx2::IPCStatus error) {
        if(error != pcsx2::NoConnection) {
            cout << "Pine Error: " << static_cast<int>(error) << endl;
            throw std::runtime_error("Pine Error");
        }   
    }
    return false;
}

void connect() {
    if(ipc) {
        return;
    }

    ipc = std::make_unique<PINE::PCSX2>();
}

void disconnect() {
    if(ipc) {
        ipc.reset();
    }
}

string get_game_id() {
    if(!is_connected()) {
        throw runtime_error("No connection to pcsx2");
    }

    return ipc->GetGameID();
}

const std::vector<unsigned char> read_bytes(uint32_t address, size_t num_of_bytes) {    
    if(address + num_of_bytes > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to read outside PS2 memory range");
    }

    if(!is_connected()) {
        throw runtime_error("No connection to pcsx2");
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
            throw runtime_error("No connection to pcsx2");
        }
        throw error;
    }
}

void write_bytes(uint32_t address, std::vector<unsigned char> data) {
    if(address + data.size() > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to write outside PS2 memory range");
    }

    if(!is_connected()) {
        throw runtime_error("No connection to pcsx2");
    }

    try {
        // TODO: should utilize pine batch write for larger write requests
        std::vector<unsigned char> result;
        for(int i = 0; i < data.size(); ++i) {
            ipc->Write<uint8_t>(address + i, data[i]);
        }
    } catch (pcsx2::IPCStatus error) {
        if (error == pcsx2::NoConnection) {
            throw runtime_error("No connection to pcsx2");
        }
        throw error;
    }
}

optional<uint32_t> find_first(const std::vector<unsigned char> seq, uint32_t start_adr, uint32_t end_adr) {
    uint32_t current_address = start_adr;
    while(current_address < end_adr) {
        vector<unsigned char> batch{};
        if(current_address + BYTES_PER_BATCH < end_adr) {
            batch = batch_read(current_address, current_address + BYTES_PER_BATCH);
        } else {
            batch = batch_read(current_address, end_adr);
        }
        
        auto res = search(batch.begin(), batch.end(), seq.begin(), seq.end());
        if(res != batch.end()) {
            return make_optional(current_address + distance(batch.begin(), res));
        }
        current_address += batch.size();
    }
    return nullopt;
}

const std::vector<unsigned char> batch_read(uint32_t start_adr, uint32_t end_adr) {
    if(start_adr >= PS2_MEMORY_SIZE || end_adr > PS2_MEMORY_SIZE) {
        throw std::out_of_range("Tried to read outside PS2 memory range");
    }

    const int byte_count = end_adr - start_adr;
    if(byte_count <= 0) {
        throw std::length_error("Non-positive range");
    }
    if(byte_count > 102400) {
        throw std::length_error("Exceeded max batch size");
    }

    if(!is_connected()) {
        throw runtime_error("No connection to pcsx2");
    }

    size_t batch_remainder = byte_count % 8;
    
    ipc->InitializeBatch();
    for(int i = start_adr; i < start_adr + byte_count - batch_remainder; i += 8) {
        ipc->Read<uint64_t, true>(i);
    }

    auto resr = ipc->FinalizeBatch();
    ipc->SendCommand(resr);

    unsigned char buffer[byte_count - batch_remainder];
    for(int i = 0; i < byte_count / 8; ++i) {
        reinterpret_cast<uint64_t*>(buffer)[i] = ipc->GetReply<pcsx2::MsgRead64>(resr, i);
    }

    vector result(buffer, buffer + byte_count - batch_remainder);
    auto last_bytes{read_bytes(end_adr - batch_remainder, batch_remainder)};
    result.insert(result.end(), last_bytes.begin(), last_bytes.end());

    return result;
}

} // namespace PCSX2Interface
