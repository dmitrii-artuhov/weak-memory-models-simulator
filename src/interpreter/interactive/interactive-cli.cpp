#include "interactive-cli.h"

#include "utils/utils.h"

#include <iostream>
#include <string>
#include <cctype>

#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"

namespace wmm_simulator {

template<class T>
bool InteractiveCli<T>::should_change_thread(const Interpreter& interpreter, const ProgramState& state) {
    if (state.threads.count(state.current_thread_id)) {
        int instruction = state.threads.at(state.current_thread_id).instruction_index;

        std::cout << "* Instruction: t " << state.current_thread_id << "> " << m_stringifier.stringify(
            interpreter.get_ast()->get_statements()[instruction].get()
        ) << std::endl;
    }

    std::cout << "* Perform thread interleaving (current thread id: " << state.current_thread_id << ")? (" << YES << "/" << NO << ")" << std::endl;

    return get_yes_no();
}

template<class T>
bool InteractiveCli<T>::should_perform_eps_transition() const {
    std::cout << "* Perform eps-transition? (" << YES << "/" << NO << ")" << std::endl;

    return get_yes_no();
}

template<class T>
int InteractiveCli<T>::get_new_thread_id(const ProgramState& state) const {
    auto thread_ids = state.get_active_threads_ids();
    
    std::cout << "* Choose new thread id. Available active threads: [";
    for (size_t i = 0; i < thread_ids.size(); ++i) {
        int id = thread_ids[i];
        if (state.current_thread_id == id) continue;

        std::cout << id;
        if (i != thread_ids.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    int choose_thread_id = 0;
    std::string ans;
    print_caret();
    while (std::getline(std::cin, ans)) {
        try {
            ans = utils::trim(ans);
            int thread_id = std::stoi(ans);

            if (std::find(thread_ids.begin(), thread_ids.end(), thread_id) == thread_ids.end()) {
                throw std::runtime_error("Cannot interleave to thread with id " + std::to_string(thread_id));
            }

            choose_thread_id = thread_id;
            break;
        }
        catch (const std::invalid_argument&) {
            std::cout << "Invalid input: 'Cannot parse integer'. Try again." << std::endl;
            print_caret();
        }
        catch (const std::out_of_range&) {
            std::cout << "Invalid input: 'Integer overflow'. Try again." << std::endl;
            print_caret();
        }
        catch (const std::exception& err) {
            std::cout << "Invalid input: '" << err.what() << "'. Try again." << std::endl;
            print_caret();
        }
    }

    return choose_thread_id;
}

template<class T>
ProgramState InteractiveCli<T>::get_new_eps_transition_state(const ProgramState& state) const {
    auto eps_transitions = state.get_eps_transition_states();
    if (eps_transitions.empty()) {
        return state;
    }

    return eps_transitions[0];
}

template<>
ProgramState InteractiveCli<PSOStorageSubsystem>::get_new_eps_transition_state(const ProgramState& state) const {
    // TODO: implement choosing location for propagation
    std::cout << "PSO Storage subsystem can choose on what location to run propagation" << std::endl;
    PSOStorageSubsystem* pso_storage = static_cast<PSOStorageSubsystem*>(state.storage.get());

    const auto locations = pso_storage->get_propagate_locations(state.current_thread_id);
    std::cout << "* Choose location for eps-transition. Available locations: [";
    for (size_t i = 0; i < locations.size(); ++i) {
        auto loc = locations[i];

        std::cout << loc;
        if (i != locations.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    std::string chosen_location;
    print_caret();
    while (std::getline(std::cin, chosen_location)) {
        chosen_location = utils::trim(chosen_location);
        std::cout << "Chosen location: '" << chosen_location << "'" << std::endl;
        
        if (std::find(locations.begin(), locations.end(), chosen_location) == locations.end()) {
            std::cout << "Invalid input: '" << chosen_location << "' does not exist. Try again." << std::endl;
            print_caret();
        }
        else {
            break;
        }
    }

    pso_storage->propagate(state.current_thread_id, chosen_location);
    return state;
}

template<class T>
void InteractiveCli<T>::print_caret() {
    std::cout << "> ";
    std::cout.flush();
}

template<class T>
bool InteractiveCli<T>::get_yes_no() const {
    std::string ans;
    bool is_yes = false;

    print_caret();
    while (std::getline(std::cin, ans)) {
        ans = utils::trim(ans);

        if (ans == YES) {
            is_yes = true;
            break;
        }
        else if (ans.empty() || ans == NO) {
            is_yes = false;
            break;
        }
        else {
            std::cout << "Unable to parse: '" << ans << "'. Try again." << std::endl;
            print_caret();
        }
    }

    return is_yes;
}

template class InteractiveCli<SCStorageSubsystem>;
template class InteractiveCli<TSOStorageSubsystem>;
template class InteractiveCli<PSOStorageSubsystem>;

}