#include "non-deterministic.h"

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
void NonDeterministicInterpreter::run() {
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
                // std::cout << "Interleave thread: " << state.current_thread_id << " -> " << new_thread_id << std::endl;
                state = state.get_interleaving_state(new_thread_id);
            }
        }

        if (state.has_eps_transitions(state.current_thread_id) && utils::get_random_in_range(0, 1) == 0) {
            // eps-transitions
            // std::cout << "Perform eps-transition on thread: " << state.current_thread_id << std::endl;
            auto eps_transition_states = state.get_eps_transition_states();
            state = eps_transition_states[utils::get_random_in_range(0, eps_transition_states.size() - 1)];
        }
        else {
            // regular transition
            // std::cout << "Perform regular transition on thread: " << state.current_thread_id << std::endl;
            state = state.get_transition_state();
        }
    }

    // std::cout << "Flush memory" << std::endl;
    state.storage->finish();

    // std::cout << "Program finished" << std::endl;

    print_state(state);
}

template void NonDeterministicInterpreter::run<SCStorageSubsystem>();
template void NonDeterministicInterpreter::run<TSOStorageSubsystem>();
template void NonDeterministicInterpreter::run<PSOStorageSubsystem>();

}