#pragma once

#include <functional>

#include "state/program-state.h"
#include "utils/node-stringifier/node-stringifier.h"

namespace wmm_simulator {

template<class T>
class InteractiveCli {
    static_assert(
        std::is_base_of<StorageSubsystem, T>::value,
        "T must be a class derived from `StorageSubsystem`."
    );

public:
    bool should_change_thread(const Interpreter& interpreter, const ProgramState& state);
    bool should_perform_eps_transition() const;

    int get_new_thread_id(const ProgramState& state) const;
    ProgramState get_new_eps_transition_state(const ProgramState& state) const;

private:
    inline static const std::string YES = "y";
    inline static const std::string NO = "n";

    static void print_caret();
    bool get_yes_no() const;

    utils::NodeStringifier m_stringifier;
};

}