#include "sc-storage-subsystem.h"

#include <string>

namespace wmm_simulator {


int SCStorageSubsystem::read(std::string_view location_name, MemoryOrder) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    if (!m_memory.count(location_name)) {
        return 0;
    }
    return m_memory[location_name];
}

void SCStorageSubsystem::write(std::string_view location_name, int value, MemoryOrder) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    m_memory[location_name] = value;
}

int SCStorageSubsystem::read_modify_write(std::string_view location_name, int new_value, MemoryOrder) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    int value = (!m_memory.count(location_name) ? 0 : m_memory[location_name]);
    m_memory[location_name] = new_value;
    return value;
}

void SCStorageSubsystem::fence(MemoryOrder) {
    // only supports MemoryOrder::SEQUENTIALLY_CONSISTENT
    // no-op for fence
}

}