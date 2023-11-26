#include "model-checking.h"

#include <algorithm>
#include <vector>
#include <set>
#include <queue>
#include <functional>

#include "state/program-state.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"
#include "utils/node-evaluator/node-evaluator.h"
#include "utils/node-stringifier/node-stringifier.h"

namespace wmm_simulator {

template<class T>
void ModelCheckingInterpreter::run() {
    static_assert(
        std::is_base_of<StorageSubsystem, T>::value,
        "T must be a class derived from `StorageSubsystem`."
    );

    std::vector<ProgramState> final_states;
    std::queue<ProgramState> states;

    ProgramState initial_state(std::move(std::make_unique<T>()), this);
    initial_state.add_thread(0);

    states.push(initial_state);
    int i = 0;

    while (!states.empty()) { // && i++ < 40
        i++;
        auto state = states.front();
        states.pop();

        // final state found
        if (!state.has_active_threads()) {
            state.storage->finish();

            // TODO: add comparison for StorageSubsystems as well
            auto it = std::find_if(final_states.begin(), final_states.end(), [&](const ProgramState& curr) {
                bool equal = true;
                
                for (const auto& [ thread_id, thread_subsystem ] : state.finished_thread_states) {
                    if (!curr.finished_thread_states.count(thread_id) || curr.finished_thread_states.at(thread_id) != thread_subsystem) {
                        equal = false;
                        break;
                    }
                }

                return equal;
            });

            if (it == final_states.end()) {
                // std::cout << "New final state:" << std::endl;
                // std::cout << state.storage->get_printable_state() << std::endl;
                // if (state.threads.count(state.current_thread_id)) {
                //     std::cout << "Registers: " << std::endl << state.threads[state.current_thread_id].thread_subsystem.get_printable_state() << std::endl;
                // }

                final_states.push_back(state);
            }

            continue;
        }

        // here we want to disallow branching from finished thread
        // if (!state.threads.count(state.current_thread_id)) {
        //     continue;
        // }

        // if (state.threads.count(state.current_thread_id)) {
        //     // std::cout << "t " << state.current_thread_id
        //     //     << "> current instruction: "
        //     //     << state.threads[state.current_thread_id].instruction_index + 1
        //     //     << "/" << get_ast()->get_statements().size() << std::endl;
        // }

        if (state.is_interleaving_possible()) {
            auto thread_ids = state.get_active_threads_ids();

            for (int new_active_thread_id : thread_ids) {
                if (state.current_thread_id == new_active_thread_id) continue;

                // std::cout << "New next state: interleave from " << state.current_thread_id << " to " << new_active_thread_id << std::endl;

                auto new_state = ProgramState(state);
                new_state.is_allowed_interleaving = false;
                new_state.current_thread_id = new_active_thread_id;
                states.push(new_state);
            }
        }

        if (!state.threads.count(state.current_thread_id)) continue;

        if (state.has_eps_transitions()) {
            auto eps_transition_states = state.get_eps_transition_states();
            // std::cout << "t " << state.current_thread_id << "> " << "Add new eps transition states (count): " << eps_transition_states.size() << std::endl;

            for (auto& new_state : eps_transition_states) {
                new_state.is_allowed_interleaving = true;
                states.push(new_state);
            }
        }

        // std::cout << "t " << state.current_thread_id << "> " << "Add new regular transition state" << std::endl;
        auto next_state = state.get_transition_state();
        // if (state.threads.at(state.current_thread_id).instruction_index == 11) {
        //     // std::cout << "Prev step had last instruction, non-finished threads afterwards: " << std::endl;
        //     for (auto [ thread_id, data ] : next_state.threads) {
        //         std::cout << "{" << thread_id << ": " << data.instruction_index << "}" << ' ';
        //     }
        //     std::cout << std::endl;
        // }
        // next_state.is_allowed_interleaving = true;
        states.push(next_state);

        // std::cout << "Total states modelings: " << states.size() << ", final states found: " << final_states.size() << std::endl;
    }

    std::cout << "Total states generated: " << i << std::endl;
    std::cout << "Final states count (unique thread subsystems states): " << final_states.size() << std::endl;

    for (const auto& final_state : final_states) {
        print_state(final_state);
    }
}

void ModelCheckingInterpreter::visit(const StatementNode* node, ProgramState* state) {
    state->is_allowed_interleaving = m_evaluator.is_allowed_interleaving(node);
    
    Interpreter::visit(node, state);
}


template void ModelCheckingInterpreter::run<SCStorageSubsystem>();
template void ModelCheckingInterpreter::run<TSOStorageSubsystem>();
template void ModelCheckingInterpreter::run<PSOStorageSubsystem>();

}