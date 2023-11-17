#include "thread-subsystem.h"

namespace wmm_simulator {

void ThreadSubsystem::set(std::string_view register_name, int value) {
    m_store[register_name] = value;
}

int ThreadSubsystem::get(std::string_view register_name) {
    return m_store[register_name];    
}

std::map<std::string, int> ThreadSubsystem::get_registers() const {
    std::map<std::string, int> result;

    for (auto [ register_name, value ] : m_store) {
        result.insert({ std::string(register_name), value });
    }

    return result;
}

}