#include "thread-subsystem.h"

#include <iostream>
#include <sstream>

namespace wmm_simulator {

void ThreadSubsystem::set(std::string_view register_name, int value) {
    m_store[register_name.data()] = value;
}

int ThreadSubsystem::get(std::string_view register_name) const {
    return m_store.at(register_name.data());    
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


bool operator==(const ThreadSubsystem& t1, const ThreadSubsystem& t2) {
    auto cmp = [](const ThreadSubsystem& from, const ThreadSubsystem& to) -> bool {
        bool equal = true;
        for (auto it = from.m_store.begin(); it != from.m_store.end() && equal; ++it) {
            const auto& [ register_name, value ] = *it;
            if (
                !to.m_store.count(register_name) ||
                to.m_store.at(register_name) != value
            ) {
                equal = false;
            }
        }

        return equal;
    };

    return cmp(t1, t2) && cmp(t2, t1);
}

bool operator!=(const ThreadSubsystem& t1, const ThreadSubsystem& t2) {
    return !(t1 == t2);
}

}