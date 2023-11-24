#pragma once

#include <memory>
#include <type_traits>
#include <map>
#include <vector>

#include "interpreter/interpreter.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"


namespace wmm_simulator {

struct ProgramState {    
    struct ThreadState {
        int instruction_index;
        ThreadSubsystem thread_subsystem;
    };

    explicit ProgramState(std::unique_ptr<StorageSubsystem>&& storage_, Interpreter* interpreter_);
    ProgramState(const ProgramState& other);
    ~ProgramState(); // empty destruction is ok here
    ProgramState& operator=(const ProgramState& other);

    void update_current_thread_instruction();
    void update_current_thread_register(std::string_view register_name);
    void add_thread(int instruction_index);

    bool has_active_threads() const;
    bool is_interleaving_possible() const;
    bool has_eps_transitions();
    std::vector <int> get_active_threads_ids() const;

    ProgramState get_interleaving_state(int new_active_thread_id);
    ProgramState get_transition_state();
    std::vector <ProgramState> get_eps_transition_states();


    int next_thread_id = 0;
    int current_thread_id = 0;
    int last_evaluated_value = 0;
    int goto_instruction = -1;
    bool is_allowed_interleaving = true;
    std::unique_ptr<StorageSubsystem> storage;
    std::map<int, ThreadState> threads;
    std::map<int, ThreadSubsystem> finished_thread_states;

private:
    Interpreter* m_interpreter;
};

}