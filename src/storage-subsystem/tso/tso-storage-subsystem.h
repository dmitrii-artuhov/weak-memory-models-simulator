#pragma once

#include "storage-subsystem/storage-subsystem.h"

#include <iostream>
#include <deque>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace wmm_simulator {

class TSOStorageSubsystem : public StorageSubsystem {
public:
    TSOStorageSubsystem();
    TSOStorageSubsystem(const TSOStorageSubsystem& other);

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
    void finish() override;

    std::vector <std::unique_ptr<StorageSubsystem>> get_eps_transitions(int thread_id) const override;

    StorageSubsystem* make_copy() const override;
    bool has_eps_transitions(int thread_id) const override;
    
    std::string get_printable_state() override;
    
    void propagate(int thread_id);
    void flush_all_buffers();

private:
    std::unordered_map<std::string, int> m_memory; // { location name, value }
    std::unordered_map<
        int,
        std::deque<std::pair<std::string, int>>
    > m_store_buffers; // { thread_id, { location name, value } }

    void flush(int thread_id);
};

}