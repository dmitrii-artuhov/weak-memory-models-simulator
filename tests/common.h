#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "state/program-state.h"
#include "thread-subsystem/thread-subsystem.h"

namespace doctest {

using namespace wmm_simulator;

inline const std::string test_data_path = "../tests/test-data"; // relative to the generated build folder


inline ThreadSubsystem create_thread_subsystem(std::initializer_list<std::pair<std::string, int>> data) {
    ThreadSubsystem t;

    for (const auto& [ register_name, value ] : data) {
        t.set(register_name, value);
    }

    return t;
}

inline bool has_threads_subsystems(const std::vector <ProgramState>& states, const std::unordered_map<int, ThreadSubsystem>& ts_expected) {
    bool found = false;

    for (const auto& s : states) {
        // std::cout << "finished threads count: " << s.finished_thread_states.size() << std::endl;
        // std::cout << "ts_expected size: " << ts_expected.size() << std::endl;
        if (s.finished_thread_states.size() != ts_expected.size()) continue;
        
        bool matched_state = true;

        for (const auto& [ thread_id, ts_got ] : s.finished_thread_states) {
            // std::cout << "Thread id: " << thread_id << std::endl;
            // std::cout << ts_got.get_printable_state() << std::endl;
            
            if (!ts_expected.count(thread_id)) {
                matched_state = false;
            }

            if (ts_expected.at(thread_id) != ts_got) {
                // std::cout << "ts_got: " << ts_got.get_printable_state() << std::endl;
                // std::cout << "ts_expected[" << thread_id << "]: " << ts_expected.at(thread_id).get_printable_state() << std::endl;
                matched_state = false;
            }

            if (!matched_state) break;
        }

        if (matched_state) found = true;
    }

    return found;
}

}