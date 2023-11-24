#include "thread-subsystem.h"

#include <iostream>
#include <sstream>

namespace wmm_simulator {

void ThreadSubsystem::set(std::string_view register_name, int value) {
    m_store[register_name] = value;
}

int ThreadSubsystem::get(std::string_view register_name) {
    return m_store[register_name];    
}

std::string ThreadSubsystem::get_printable_state() const {
    std::stringstream ss;
    
    if (m_store.empty()) ss << "[empty]" << std::endl;

    for (const auto& [ loc, val ] : m_store) {
        ss << loc << ": " << val << std::endl;
    }
    ss << std::endl;

    return ss.str();
}

std::map<std::string, int> ThreadSubsystem::get_registers() const {
    std::map<std::string, int> result;

    for (auto [ register_name, value ] : m_store) {
        result.insert({ std::string(register_name), value });
    }

    return result;
}

}