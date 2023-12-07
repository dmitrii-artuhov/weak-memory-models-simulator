#include "interactive.h"

#include <functional>

#include "interactive-cli.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"
#include "storage-subsystem/sra/sra-storage-subsystem.h"

namespace wmm_simulator {

template<class T>
void InteractiveInterpreter::run() {
    static_assert(
        std::is_base_of<StorageSubsystem, T>::value,
        "T must be a class derived from `StorageSubsystem`."
    );
    
    ProgramState state(std::move(std::make_unique<T>()), this);
    state.add_thread(0);
    InteractiveCli<T> cli;

    while (state.has_active_threads()) {
        // interleave thread
        if (
            state.is_interleaving_possible() &&
            (!state.threads.count(state.current_thread_id) || cli.should_change_thread(*this, state))
        ) {
            int new_thread_id = cli.get_new_thread_id(state /* excludes current_thread_id */);
            std::cout << get_log_prefix(state.current_thread_id) << "interleave to thread: " << new_thread_id << std::endl;

            state = state.get_interleaving_state(new_thread_id);
        }

        if (
            state.has_eps_transitions() &&
            cli.should_perform_eps_transition()
        ) {
            state = cli.get_new_eps_transition_state(state);
            
            std::cout << get_log_prefix(state.current_thread_id) << "perform eps-transition" << std::endl;
        }
        else {
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
    state.storage->flush();

    std::cout << "Program finished" << std::endl;

    print_state(state);
};


template void InteractiveInterpreter::run<SCStorageSubsystem>();
template void InteractiveInterpreter::run<TSOStorageSubsystem>();
template void InteractiveInterpreter::run<PSOStorageSubsystem>();
template void InteractiveInterpreter::run<SRAStorageSubsystem>();

}