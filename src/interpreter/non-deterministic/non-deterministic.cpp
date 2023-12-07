#include "non-deterministic.h"

#include <vector>
#include <iostream>


#include "state/program-state.h"
#include "utils/utils.h"
#include "exceptions/exceptions.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"
#include "storage-subsystem/sra/sra-storage-subsystem.h"


namespace wmm_simulator {

template<class T>
std::vector<ProgramState> NonDeterministicInterpreter::run() {
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
                state = state.get_interleaving_state(new_thread_id);
            }
        }

        if (state.has_eps_transitions() && utils::get_random_in_range(0, 1) == 0) {
            // eps-transitions
            auto eps_transition_states = state.get_eps_transition_states();
            state = eps_transition_states[utils::get_random_in_range(0, eps_transition_states.size() - 1)];
        }
        else {
            // regular transition
            state = state.get_transition_state();
        }
    }

    state.storage->flush();
    print_state(state);

    return { state };
}

template std::vector<ProgramState> NonDeterministicInterpreter::run<SCStorageSubsystem>();
template std::vector<ProgramState> NonDeterministicInterpreter::run<TSOStorageSubsystem>();
template std::vector<ProgramState> NonDeterministicInterpreter::run<PSOStorageSubsystem>();
template std::vector<ProgramState> NonDeterministicInterpreter::run<SRAStorageSubsystem>();

}