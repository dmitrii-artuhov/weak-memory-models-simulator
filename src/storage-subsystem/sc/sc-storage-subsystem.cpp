#include "sc-storage-subsystem.h"

#include <string>
#include <sstream>
#include <map>

namespace wmm_simulator {

SCStorageSubsystem::SCStorageSubsystem() {}

SCStorageSubsystem::SCStorageSubsystem(const SCStorageSubsystem& other) {
    m_memory = other.m_memory;
}

int SCStorageSubsystem::read(
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

void SCStorageSubsystem::write(
    int,
    std::string_view location_name,
    int value,
    MemoryOrder
) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    m_memory[location_name] = value;
}

void SCStorageSubsystem::fence(
    int,
    MemoryOrder
) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    // no-op for fence
}

StorageSubsystem* SCStorageSubsystem::make_copy() const {
    return new SCStorageSubsystem(*this);
}

std::string SCStorageSubsystem::get_printable_state() {
    std::stringstream ss;

    ss << "Memory:" << std::endl;
    if (m_memory.empty()) ss << "[empty]" << std::endl;

    for (auto& [ loc, val ] : m_memory) {
        ss << loc << ": " << val << std::endl;
    }

    return ss.str();
}

std::map<std::string, int> SCStorageSubsystem::get_storage() {
    std::map<std::string, int> result;

    for (auto [ loc, val ] : m_memory) {
        result.insert({ std::string(loc), val });
    }

    return result;
}

}