#pragma once

#include <map>
#include <unordered_map>
#include <string>

namespace wmm_simulator {

class ThreadSubsystem {
private:
    std::map<std::string_view, int> m_store; // { register, value }

public:
    void set(std::string_view register_name, int value);
    int get(std::string_view register_name) const;
    std::string get_printable_state() const;
    std::map<std::string, int> get_registers() const;

    friend bool operator==(const ThreadSubsystem& t1, const ThreadSubsystem& t2);
    friend bool operator!=(const ThreadSubsystem& t1, const ThreadSubsystem& t2);
};

}