#pragma once

#include <unordered_map>
#include <iostream>

#include "storage-subsystem/storage-subsystem.h"

namespace wmm_simulator {

class SCStorageSubsystem : public StorageSubsystem {
public:
    ~SCStorageSubsystem() {
        // TODO: remove
        std::cout << "~SCStorageSubsystem" << std::endl;
    }

    int read(std::string_view location_name, MemoryOrder memory_order) override;
    void write(std::string_view location_name, int value, MemoryOrder memory_order) override;
    int read_modify_write(std::string_view location_name, int new_value, MemoryOrder memory_order) override;
    void fence(MemoryOrder memory_order) override;

private:
    std::unordered_map<std::string_view, int> m_memory; // { location name, value }
};

}