#include "tracing.h"

#include <vector>
#include <iostream>


#include "interpreter/state/program-state.h"
#include "utils/utils.h"
#include "exceptions/exceptions.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"


namespace wmm_simulator {

template<class T>
void TracingInterpreter::run() {
    static_assert(
        std::is_base_of<StorageSubsystem, T>::value,
        "T must be a class derived from `StorageSubsystem`."
    );
    
    ProgramState state(std::move(std::make_unique<T>()), this);
    state.add_thread(0);

    while (state.has_active_threads()) {
        // interleave thread
        if (state.is_interleaving_possible()) {
            int new_thread_id = get_random_active_thread_id(state);
            if (new_thread_id != state.current_thread_id) {
                std::cout << get_log_prefix(state.current_thread_id) << "interleave to thread: " << new_thread_id << std::endl;
                state = state.get_interleaving_state(new_thread_id);
            }
        }

        if (state.has_eps_transitions() && utils::get_random_in_range(0, 1) == 0) {
            // eps-transitions
            std::cout << get_log_prefix(state.current_thread_id) << "perform eps-transition" << std::endl;
            auto eps_transition_states = state.get_eps_transition_states();
            
            state = eps_transition_states[utils::get_random_in_range(0, eps_transition_states.size() - 1)];
        }
        else {
            // regular transition
            std::cout << get_log_prefix(state.current_thread_id) << "perform regular transition" << std::endl;
            state = state.get_transition_state();
        }

        // Print state change
        std::cout << state.storage->get_printable_state() << std::endl;
        if (state.threads.count(state.current_thread_id)) {
            std::cout << "Registers: " << std::endl << state.threads[state.current_thread_id].thread_subsystem.get_printable_state() << std::endl;
        }
    }

    std::cout << "Flush memory" << std::endl;
    state.storage->finish();

    std::cout << "Program finished" << std::endl;

    print_state(state);
}

void TracingInterpreter::visit(const StatementNode* node, ProgramState* state) {
    std::cout << get_log_prefix(state->current_thread_id) << m_stringifier.stringify(node) << std::endl << std::endl;

    Interpreter::visit(node, state);
}

std::string TracingInterpreter::get_log_prefix(int thread_id) const {
    return "t " + std::to_string(thread_id) + "> ";
}

template void TracingInterpreter::run<SCStorageSubsystem>();
template void TracingInterpreter::run<TSOStorageSubsystem>();
template void TracingInterpreter::run<PSOStorageSubsystem>();

}