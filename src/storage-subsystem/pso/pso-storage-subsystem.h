#pragma once

#include "storage-subsystem/storage-subsystem.h"

#include <iostream>
#include <deque>
#include <string>
#include <algorithm>

namespace wmm_simulator {

class PSOStorageSubsystem : public StorageSubsystem {
public:
    PSOStorageSubsystem();
    PSOStorageSubsystem(const PSOStorageSubsystem& other);

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
    void flush() override;

    std::vector <std::unique_ptr<StorageSubsystem>> get_eps_transitions(int thread_id) const override;

    StorageSubsystem* make_copy() const override;
    std::string get_printable_state() override;

    bool has_eps_transitions(int thread_id) const;
    const std::vector <std::string_view> get_propagate_locations(int thread_id) const;
    void propagate(int thread_id, const std::string& location_name);
    void flush_all_buffers();

private:
    std::unordered_map<std::string, int> m_memory; // { location name, value }
    std::unordered_map<
        int,
        std::unordered_map<
            std::string,
            std::deque<int>
        >
    > m_store_buffers; // { thread_id, { location name, [v1, v2, ...] } }

    void flush_buffer(int thread_id);
};

}