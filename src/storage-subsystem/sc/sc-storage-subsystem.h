#pragma once

#include <iostream>
#include <unordered_map>

#include "storage-subsystem/storage-subsystem.h"

namespace wmm_simulator {

class SCStorageSubsystem : public StorageSubsystem {
public:
    SCStorageSubsystem();
    SCStorageSubsystem(const SCStorageSubsystem& other);

    int read(
        int thread_id,
        const std::string& location_name,
        MemoryOrder memory_order
    ) override;
    void write(
        int thread_id,
        const std::string& location_name,
        int value,
        MemoryOrder memory_order
    ) override;
    void fence(
        int thread_id,
        MemoryOrder memory_order
    ) override;
    StorageSubsystem* make_copy() const override;

    std::string get_printable_state() override;
    std::map<std::string, int> get_storage() override;

private:
    std::map<std::string, int> m_memory; // { location name, value }
};

}