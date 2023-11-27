#include "sra-storage-subsystem.h"

#include <string>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <map>
#include <memory>

#include "utils/utils.h"


namespace wmm_simulator {

SRAStorageSubsystem::SRAStorageSubsystem() {}

SRAStorageSubsystem::SRAStorageSubsystem(const SRAStorageSubsystem& other) {
    m_memory = other.m_memory;
    m_thread_views = other.m_thread_views;
    m_location_timestamps = other.m_location_timestamps;
}

int SRAStorageSubsystem::read(
    int thread_id,
    const std::string& location_name,
    MemoryOrder memory_order
) {
    int result = 0;
    int current_thread_timestamp = m_thread_views[thread_id].get_timestamp(location_name);
    
    ThreadView* read_from_view = nullptr;    

    for (auto& [ loc, val, memory_order, thread_view ] : m_memory) {
        int timestamp = thread_view.get_timestamp(loc);

        if (loc == location_name && timestamp == current_thread_timestamp) {
            result = val;
            read_from_view = &thread_view;
            break;
        }
    }

    if (
        (
            memory_order == MemoryOrder::ACQUIRE ||
            memory_order == MemoryOrder::RELEASE_ACQUIRE
        ) &&
        read_from_view != nullptr
    ) {
        for (const auto& [ loc, new_timestamp ] : read_from_view->view) {
            auto& current_thread_view = m_thread_views[thread_id];
            
            current_thread_view.set_timestamp(
                loc,
                std::max(current_thread_view.get_timestamp(loc), new_timestamp)
            );
        }
    }

    return result;
}

void SRAStorageSubsystem::write(
    int thread_id,
    const std::string& location_name,
    int value,
    MemoryOrder memory_order
) {
    int timestamp = ++m_location_timestamps[location_name];
    m_thread_views[thread_id].set_timestamp(location_name, timestamp);

    if (memory_order == MemoryOrder::RELEASE || memory_order == MemoryOrder::RELEASE_ACQUIRE) {
        m_memory.push_back({
            location_name,
            value,
            memory_order,
            ThreadView(m_thread_views[thread_id])
        });
    }
    else {
        m_memory.push_back({
            location_name,
            value,
            memory_order,
            ThreadView(location_name, timestamp)
        });
    }
}

void SRAStorageSubsystem::fence(
    int thread_id,
    MemoryOrder memory_order
) {
    // I have a solution for REL/ACQ fences (implement them via RMW to a non-existing location with arbitrary value)
    // Did not implement on time. Can do later?

    // For SC fences, don't really know...
    throw std::runtime_error("Fences not supported for 'Strong Release Acquire' memory model");
}

bool SRAStorageSubsystem::has_eps_transitions(int thread_id) const {
    if (!m_thread_views.count(thread_id)) return false;

    for (const auto& [ loc, max_timestamp ] : m_location_timestamps) {
        int current_thread_timestamp = m_thread_views.at(thread_id).get_timestamp(loc);

        if (current_thread_timestamp < max_timestamp) {
            return true;
        }
    }
    
    return false;
}

std::vector <std::unique_ptr<StorageSubsystem>> SRAStorageSubsystem::get_eps_transitions(int thread_id) const {
    std::vector <std::unique_ptr<StorageSubsystem>> results;

    if (has_eps_transitions(thread_id)) {
        const auto increase_locations = get_increase_locations(thread_id);
        
        for (const auto& loc : increase_locations) {
            SRAStorageSubsystem* new_storage = new SRAStorageSubsystem(*this);
            new_storage->increase_location_local_timestamp(thread_id, loc.data());

            results.push_back(std::unique_ptr<StorageSubsystem>(new_storage));
        }

    }

    return results;
}

const std::vector <std::string_view> SRAStorageSubsystem::get_increase_locations(int thread_id) const {
    std::vector <std::string_view> results;
    
    for (const auto& [ loc, max_timestamp ] : m_location_timestamps) {
        if (
            (!m_thread_views.count(thread_id) && 0 < max_timestamp) ||
            (m_thread_views.at(thread_id).get_timestamp(loc) < max_timestamp)
        ) {
            results.push_back(loc);
        }
    }

    return results;
}

void SRAStorageSubsystem::increase_location_local_timestamp(int thread_id, const std::string& location_name) {
    int prev_timestamp = m_thread_views[thread_id].get_timestamp(location_name);

    if (prev_timestamp < m_location_timestamps[location_name]) {
        m_thread_views[thread_id].set_timestamp(location_name, prev_timestamp + 1);
    }
}

StorageSubsystem* SRAStorageSubsystem::make_copy() const {
    return new SRAStorageSubsystem(*this);
}

std::string SRAStorageSubsystem::get_printable_state() {
    std::stringstream ss;

    ss << "Memory:" << std::endl;
    if (m_memory.empty()) ss << "[empty]" << std::endl;

    for (const auto& [ loc, val, memory_order, thread_view ] : m_memory) {
        ss << "<" << utils::get_memory_order(memory_order) << "; " << loc << ": " << val << " @ " << thread_view.get_timestamp(loc) << ">" << std::endl;
    }

    ss << "Global timestamps:" << std::endl;
    if (m_location_timestamps.empty()) ss << "[empty]" << std::endl;

    for (const auto& [ loc, max_timestamp ] : m_location_timestamps) {
        ss << loc << " @ " << max_timestamp << std::endl;
    }

    ss << "Thread views:" << std::endl;
    if (m_thread_views.empty()) ss << "[empty]";

    for (auto& [ thread_id, view ] : m_thread_views) {
        ss << "Thread " << thread_id << ": " << std::endl;
        ss << view.get_printable_state();
    }

    return ss.str();
}

void SRAStorageSubsystem::prepare_thread(int thread_id) {
    m_thread_views.insert({ thread_id, ThreadView() });
}

}