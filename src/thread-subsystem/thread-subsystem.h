#pragma once

#include <unordered_map>
#include <string>

namespace wmm_simulator {

class ThreadSubsystem {
private:
    std::unordered_map<std::string_view, int> m_store; // { register, value }

public:
    void set(std::string_view register_name, int value);
    int get(std::string_view register_name);
};

}