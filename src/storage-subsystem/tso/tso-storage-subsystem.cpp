#include "tso-storage-subsystem.h"

#include <string>
#include <sstream>
#include <cassert>
#include <map>

namespace wmm_simulator {

TSOStorageSubsystem::TSOStorageSubsystem() {}

TSOStorageSubsystem::TSOStorageSubsystem(const TSOStorageSubsystem& other) {
    m_memory = other.m_memory;
    m_store_buffers = other.m_store_buffers;
}

int TSOStorageSubsystem::read(
    int thread_id,
    std::string_view location_name,
    MemoryOrder
) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    bool found = false;
    int result = 0;

    if (m_store_buffers.count(thread_id)) {
        auto& d = m_store_buffers[thread_id];

        for (int i = d.size() - 1; i >= 0; i--) {
            auto [ loc, val ] = d[i];

            if (loc == location_name) {
                found = true;
                result = val;
                break;
            }
        }
    }
    
    if (!found && m_memory.count(location_name)) {
        result = m_memory[location_name];
    }

    return result;
}

void TSOStorageSubsystem::write(
    int thread_id,
    std::string_view location_name,
    int value,
    MemoryOrder memory_order
) {
    m_store_buffers[thread_id].push_back({ location_name, value });

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

std::string TSOStorageSubsystem::get_printable_state() {
    std::stringstream ss;

    ss << "Memory:" << std::endl;
    if (m_memory.empty()) ss << "[empty]" << std::endl;

    for (auto& [ loc, val ] : m_memory) {
        ss << loc << ": " << val << std::endl;
    }

    ss << "Store buffers: " << std::endl;
    if (m_store_buffers.empty()) ss << "[empty]";

    for (auto& [ thread_id, q ] : m_store_buffers) {
        ss << "Thread " << thread_id << ": ";

        for (auto it = q.begin(); it != q.end(); ++it) {
            ss << "{" << it->first << ", " << it->second << "}";

            if (it + 1 != q.end()) {
                ss << ", ";
            }
        }

        ss << std::endl;
    }

    return ss.str();
}

StorageSubsystem* TSOStorageSubsystem::make_copy() const {
    return new TSOStorageSubsystem(*this);
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
    q.pop_front();
    if (q.empty()) {
        m_store_buffers.erase(thread_id);
    }

    m_memory[location_name] = value;
}

void TSOStorageSubsystem::flush_all_buffers() {
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
        q.pop_front();
        m_memory[location_name] = value;
    }

    m_store_buffers.erase(thread_id);
}

}