#include "sc-storage-subsystem.h"

#include <string>

namespace wmm_simulator {


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

std::unordered_map<std::string, int> SCStorageSubsystem::get_storage() {
    std::unordered_map<std::string, int> result;

    for (auto [ loc, val ] : m_memory) {
        result.insert({ std::string(loc), val });
    }

    return result;
}

}