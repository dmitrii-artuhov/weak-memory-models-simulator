#pragma once

#include "storage-subsystem/storage-subsystem.h"

#include <iostream>
#include <queue>
#include <string>
#include <algorithm>

namespace wmm_simulator {

class TSOStorageSubsystem : public StorageSubsystem {
public:
    ~TSOStorageSubsystem() {
        std::cout << "~TSOStorageSubsystem" << std::endl;
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
    
    bool has_eps_transitions(int thread_id) const;
    void propagate(int thread_id);
    void flush_all_buffers();
    std::map<std::string, int> get_storage() override;

private:
    std::unordered_map<std::string_view, int> m_memory; // { location name, value }
    std::unordered_map<
        int,
        std::queue<std::pair<std::string_view, int>>
    > m_store_buffers; // { thread_id, { location name, value } }

    void flush(int thread_id);
};

}