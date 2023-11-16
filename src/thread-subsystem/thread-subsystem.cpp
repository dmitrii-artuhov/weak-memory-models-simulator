#include "thread-subsystem.h"

namespace wmm_simulator {

void ThreadSubsystem::set(std::string_view register_name, int value) {
    m_store[register_name] = value;
}

int ThreadSubsystem::get(std::string_view register_name) {
    return m_store[register_name];    
}

}