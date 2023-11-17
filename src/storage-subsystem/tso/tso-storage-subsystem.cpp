#include "tso-storage-subsystem.h"

#include <string>
#include <cassert>
#include <map>

namespace wmm_simulator {

int TSOStorageSubsystem::read(
    int,
    std::string_view location_name,
    MemoryOrder
) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    if (!m_memory.count(location_name)) {
        return 0;
    }

    return m_memory[location_name];
}

void TSOStorageSubsystem::write(
    int thread_id,
    std::string_view location_name,
    int value,
    MemoryOrder memory_order
) {
    m_store_buffers[thread_id].push({ location_name, value });

    // TODO: should any weaker memory orders perform the same?
    if (memory_order == MemoryOrder::SEQUENTIALLY_CONSISTENT) {
        flush(thread_id);
    }
}

void TSOStorageSubsystem::fence(
    int thread_id,
    MemoryOrder memory_order
) {
    // TODO: should other memory orders be supported for fences?
    if (memory_order == MemoryOrder::SEQUENTIALLY_CONSISTENT) {
        flush(thread_id);
    }
}

std::map<std::string, int> TSOStorageSubsystem::get_storage() {
    std::map<std::string, int> result;
    
    for (auto& [ location_name, value ] : m_memory) {
        result.insert({ std::string(location_name), value });
    }

    return result;
}


bool TSOStorageSubsystem::has_eps_transitions(int thread_id) const {
    return m_store_buffers.count(thread_id);
}

void TSOStorageSubsystem::propagate(int thread_id) {
    if (!m_store_buffers.count(thread_id)) {
        return;
    }

    auto& q = m_store_buffers[thread_id];
    assert(("Queue must not be empty when performing propagate on thread in TSO: " + std::to_string(thread_id), !q.empty()));
    
    auto [ location_name, value ] = q.front();
    q.pop();
    if (q.empty()) {
        m_store_buffers.erase(thread_id);
    }

    m_memory[location_name] = value;
}

void TSOStorageSubsystem::flush_all_buffers() {
    std::cout << "Flush buffers" << std::endl;
    std::vector <int> thread_ids;
    for (auto& [ thread_id, _ ] : m_store_buffers) {
        thread_ids.push_back(thread_id);
    }

    for (int thread_id : thread_ids) {
        flush(thread_id);
    }
}

void TSOStorageSubsystem::flush(int thread_id) {
    if (!m_store_buffers.count(thread_id)) {
        return;
    }

    auto& q = m_store_buffers[thread_id];
    while (!q.empty()) {
        auto [ location_name, value ] = q.front();
        q.pop();
        m_memory[location_name] = value;
    }

    m_store_buffers.erase(thread_id);
}

}