#pragma once

#include <iostream>
#include <unordered_map>

#include "storage-subsystem/storage-subsystem.h"

namespace wmm_simulator {

class SCStorageSubsystem : public StorageSubsystem {
public:
    ~SCStorageSubsystem() {
        // TODO: remove
        std::cout << "~SCStorageSubsystem" << std::endl;
    }

    int read(
        int thread_id,
        std::string_view location_name,
        MemoryOrder memory_order
    ) override;
    void write(
        int thread_id,
        std::string_view location_name,
        int value,
        MemoryOrder memory_order
    ) override;
    void fence(
        int thread_id,
        MemoryOrder memory_order
    ) override;
    
    std::string get_printable_state() override;
    std::map<std::string, int> get_storage() override;

private:
    std::map<std::string_view, int> m_memory; // { location name, value }
};

}