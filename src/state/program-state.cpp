#include "program-state.h"

#include "exceptions/exceptions.h"
#include "interpreter/interpreter.h"

namespace wmm_simulator {

ProgramState::ProgramState(
    std::unique_ptr<StorageSubsystem>&& storage_,
    Interpreter* interpreter_
): storage(std::move(storage_)), m_interpreter(interpreter_) {}


ProgramState::ProgramState(const ProgramState& other) {
    *this = other;
}

ProgramState::~ProgramState() {}


ProgramState& ProgramState::operator=(const ProgramState& other) {
    if (this == &other) {
        return *this;
    }
    
    next_thread_id = other.next_thread_id;
    current_thread_id = other.current_thread_id;
    last_evaluated_value = other.last_evaluated_value;
    goto_instruction = other.goto_instruction;
    is_allowed_interleaving = other.is_allowed_interleaving;
    storage = std::unique_ptr<StorageSubsystem> (other.storage->make_copy());
    threads = other.threads;
    finished_thread_states = other.finished_thread_states;
    m_interpreter = other.m_interpreter;

    return *this;
}

void ProgramState::update_current_thread_instruction() {
    if (threads.count(current_thread_id)) {
        if (goto_instruction == -1) {
            threads[current_thread_id].instruction_index++;
        }
        else {
            threads[current_thread_id].instruction_index = goto_instruction;
            goto_instruction = -1;
        }
    }
}

void ProgramState::update_current_thread_register(std::string_view register_name) {
    threads[current_thread_id].thread_subsystem.set(
        register_name,
        last_evaluated_value
    );
}

void ProgramState::add_thread(int instruction_index) {
    threads[next_thread_id] = {
        instruction_index,
        ThreadSubsystem()
    };
    storage->prepare_thread(next_thread_id);
    next_thread_id++;
}

bool ProgramState::has_active_threads() const {
    return !threads.empty();
}

bool ProgramState::is_interleaving_possible() const {
    return
        is_allowed_interleaving &&
        (threads.size() > 1 || !threads.count(current_thread_id));
}

bool ProgramState::has_eps_transitions() {
    return storage->has_eps_transitions(current_thread_id);
}

std::vector <int> ProgramState::get_active_threads_ids() const {
    std::vector <int> active_threads_ids;
    
    for (const auto& [ thread_id, thread_state ] : threads) {
        active_threads_ids.push_back(thread_id);
    }

    return active_threads_ids;
}

ProgramState ProgramState::get_interleaving_state(int new_active_thread_id) const {    
    if (
        current_thread_id == new_active_thread_id ||
        !threads.count(new_active_thread_id)
    ) {
        throw exceptions::invalid_thread_interleaving(current_thread_id, new_active_thread_id);
    }

    auto new_state = ProgramState(*this);
    new_state.current_thread_id = new_active_thread_id;
    
    return new_state;
}

ProgramState ProgramState::get_transition_state() const {
    auto new_state = ProgramState(*this);
    
    int instruction = threads.at(current_thread_id).instruction_index;
    m_interpreter->get_ast()->get_statements()[instruction]->accept(
        m_interpreter, &new_state
    );
    
    return new_state;
}

std::vector<ProgramState> ProgramState::get_eps_transition_states() const {
    auto new_storages = storage->get_eps_transitions(current_thread_id);
    std::vector<ProgramState> results;
    
    for (size_t i = 0; i < new_storages.size(); ++i) {
        ProgramState new_state(*this);
        new_state.storage = std::move(new_storages[i]);
        
        results.push_back(std::move(new_state));
    }

    return results;
}

}