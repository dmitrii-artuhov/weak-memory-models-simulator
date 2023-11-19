#include "pso-storage-subsystem.h"

#include <string>
#include <sstream>
#include <cassert>
#include <map>

namespace wmm_simulator {

int PSOStorageSubsystem::read(
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

void PSOStorageSubsystem::write(
    int thread_id,
    std::string_view location_name,
    int value,
    MemoryOrder memory_order
) {
    m_store_buffers[thread_id][location_name].push_back(value);
    
    // TODO: should any weaker memory orders perform the same?
    if (memory_order == MemoryOrder::SEQUENTIALLY_CONSISTENT) {
        flush(thread_id);
    }
}

void PSOStorageSubsystem::fence(
    int thread_id,
    MemoryOrder memory_order
) {
    // TODO: should any weaker memory orders perform the same?
    if (memory_order == MemoryOrder::SEQUENTIALLY_CONSISTENT) {
        flush(thread_id);
    }
}

std::string PSOStorageSubsystem::get_printable_state() {
    std::stringstream ss;

    ss << "Memory:" << std::endl;
    if (m_memory.empty()) ss << "[empty]" << std::endl;

    for (auto& [ loc, val ] : m_memory) {
        ss << loc << ": " << val << std::endl;
    }

    ss << "Store buffers:" << std::endl;
    if (m_store_buffers.empty()) ss << "[empty]";

    for (auto& [ thread_id, buffers ] : m_store_buffers) {
        ss << "Thread " << thread_id << ": " << std::endl;

        for (auto& [ loc, q ] : buffers) {
            ss << "   " << loc << ": ";

            for (auto it = q.begin(); it != q.end(); ++it) {
                ss << *it;

                if (it + 1 != q.end()) {
                    ss << ", ";
                }
            }

            ss << std::endl;
        }
    }

    return ss.str();
}

std::map<std::string, int> PSOStorageSubsystem::get_storage() {
    std::map<std::string, int> result;
    
    for (auto& [ location_name, value ] : m_memory) {
        result.insert({ std::string(location_name), value });
    }

    return result;
}

bool PSOStorageSubsystem::has_eps_transitions(int thread_id) const {
    return m_store_buffers.count(thread_id);
}

const std::vector <std::string_view> PSOStorageSubsystem::get_propagate_locations(int thread_id) {
    if (!m_store_buffers.count(thread_id)) {
        return {};
    }

    auto& thread_buffer = m_store_buffers[thread_id];
    std::vector <std::string_view> result(thread_buffer.size());
    size_t i = 0;
    for (auto& [ location_name, q ] : thread_buffer) {
        result[i++] = location_name;
    }

    return result;
}

void PSOStorageSubsystem::propagate(int thread_id, std::string_view location_name) {
    if (!m_store_buffers.count(thread_id)) {
        return;
    }

    auto& thread_buffer = m_store_buffers[thread_id];
    
    if (!thread_buffer.count(location_name)) {
        return;
    }

    auto& q = thread_buffer[location_name];
    int value = q.front();
    q.pop_front();
    if (q.empty()) {
        thread_buffer.erase(location_name);
    }

    if (thread_buffer.empty()) {
        m_store_buffers.erase(thread_id);
    }

    m_memory[location_name] = value;
}

void PSOStorageSubsystem::flush_all_buffers() {
    std::vector <int> thread_ids;
    for (auto& [ thread_id, _ ] : m_store_buffers) {
        thread_ids.push_back(thread_id);
    }

    for (int thread_id : thread_ids) {
        flush(thread_id);
    }
}

void PSOStorageSubsystem::flush(int thread_id) {
    if (!m_store_buffers.count(thread_id)) {
        return;
    }

    auto& thread_buffer = m_store_buffers[thread_id];
    for (auto& [ location_name, q ] : thread_buffer) {
        assert(("Queue must not be empty in flush of PSO"));
        m_memory[location_name] = q.back();
    }

    m_store_buffers.erase(thread_id);
}

}