#include "interpreter.h"

#include <memory>
#include <iterator>

#include "program-state.h"
#include "utils/utils.h"
#include "exceptions/exceptions.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"
#include "ast/node.h"
// #include "utils/node-stringifier.h"


namespace wmm_simulator {


Interpreter::Interpreter(
    std::shared_ptr<ProgramNode> root,
    std::unordered_map<std::string_view, int> labeled_instructions
): m_root(root), m_labeled_instructions(std::move(labeled_instructions)) {}

const ProgramNode* Interpreter::get_ast() const {
    return m_root.get();
}

// void Interpreter::init() {
//     m_max_thread_index = 0;
//     m_current_thread = 0;
//     m_last_evaluated_value = 0;
//     m_goto_instruction = -1;
    
//     m_threads.clear();
//     m_finished_thread_states.clear();
    
//     m_storage = std::shared_ptr<StorageSubsystem>(new T());
//     // Spawn main thread
//     m_threads[0] = {
//         0, // since END instuction is always in the ast
//         ThreadSubsystem()
//     };
// }

int Interpreter::get_random_active_thread_id(const ProgramState& state) const {    
    int offset = utils::get_random_in_range(0, state.threads.size() - 1);

    auto it = state.threads.begin();
    std::advance(it, offset);
    
    std::cout << "random thread id: " << it->first << std::endl;
    return it->first;
}


/*  Specialization for a SC storage subsystem */
// TODO: use SFINAE in header file?
template<class T>
void Interpreter::run() {
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
                std::cout << "Interleave thread: " << state.current_thread_id << " -> " << new_thread_id << std::endl;
                state = state.get_interleaving_state(new_thread_id);
            }
        }

        if (state.has_eps_transitions(state.current_thread_id) && utils::get_random_in_range(0, 1) == 0) {
            // eps-transitions
            std::cout << "Perform eps-transition on thread: " << state.current_thread_id << std::endl;
            auto eps_transition_states = state.get_eps_transition_states();
            state = eps_transition_states[utils::get_random_in_range(0, eps_transition_states.size() - 1)];
        }
        else {
            // regular transition
            std::cout << "Perform regular transition on thread: " << state.current_thread_id << std::endl;
            state = state.get_transition_state();
        }
    }

    std::cout << "Program finished" << std::endl;

    /*---- Print final results of execution -----------------------------------*/
    std::cout << std::endl << "=========== Memory state ===========" << std::endl;
    std::cout << state.storage->get_printable_state() << std::endl;
    std::cout << "====================================" << std::endl;

    std::cout << std::endl << "=========== Thread states ==========" << std::endl;
    for (auto& [ thread_id, thread_subsystem ] : state.finished_thread_states) {
        std::cout << "Thread " << thread_id << std::endl;
        std::cout << thread_subsystem.get_printable_state();
    }
    std::cout << "====================================" << std::endl;

    // pick random thread
    // int offset = utils::get_random_in_range(0, m_threads.size() - 1);
    // // std::cout << "offset: " << offset << std::endl;
    
    // auto it = m_threads.begin();
    // std::advance(it, offset);
    // return it->first;

    // void Interpreter::interleave_thread() {
    // // pick current thread
    //     int prev_thread_id = m_current_thread;
    //     m_current_thread = pick_random_thread();

    //     if (prev_thread_id != m_current_thread && m_is_verbose) {
    //         std::cout << "Thread interleaving: " << prev_thread_id << " -> " << m_current_thread << std::endl << std::endl;
    //     }
    // }


    // init();

    // while (has_active_threads()) {
    //     // TODO: implement skipping of unrelated to storage subsystem operations 

    //     interleave_thread();

    //     // non-eps transitions
    //     int instruction = m_threads[m_current_thread].instruction_index;
    //     m_root->get_statements()[instruction]->accept(this);

    //     if (m_is_verbose) {
    //         std::cout << m_storage->get_printable_state() << std::endl;
            
    //         if (m_threads.count(m_current_thread)) {
    //             std::cout << "Registers:" << std::endl << m_threads[m_current_thread].thread_subsystem.get_printable_state() << std::endl;
    //         }
    //     }
    // }

    // return get_state();
}


/*  Specialization for a TSO storage subsystem */
// template<>
// std::pair<
//     std::shared_ptr<StorageSubsystem>,
//     std::map<int, ThreadSubsystem>
// > Interpreter<TSOStorageSubsystem>::run() {
//     init();

//     std::shared_ptr<TSOStorageSubsystem> tso_storage = std::static_pointer_cast<TSOStorageSubsystem> (m_storage);
    
//     while (has_active_threads()) {
//         // TODO: implement skipping of unrelated to storage subsystem operations 

//         interleave_thread();

//         if (
//             tso_storage->has_eps_transitions(m_current_thread) &&
//             utils::get_random_in_range(0, 1) == 0
//         ) {
//             // eps transitions
//             if (m_is_verbose) {
//                 std::cout << get_log_prefix() << "perform propagate" << std::endl << std::endl;
//             }
//             tso_storage->propagate(m_current_thread);
//         }
//         else {
//             // non-eps transitions
//             int instruction = m_threads[m_current_thread].instruction_index;
//             m_root->get_statements()[instruction]->accept(this);
//         }

//         if (m_is_verbose) {
//             std::cout << m_storage->get_printable_state() << std::endl;
            
//             if (m_threads.count(m_current_thread)) {
//                 std::cout << "Registers:" << std::endl << m_threads[m_current_thread].thread_subsystem.get_printable_state() << std::endl;
//             }
//         }
//     }

//     tso_storage->flush_all_buffers();

//     return get_state();
// }


// /*  Specialization for a PSO storage subsystem */
// template<>
// std::pair<
//     std::shared_ptr<StorageSubsystem>,
//     std::map<int, ThreadSubsystem>
// > Interpreter<PSOStorageSubsystem>::run() {
//     init();

//     std::shared_ptr<PSOStorageSubsystem> pso_storage = std::static_pointer_cast<PSOStorageSubsystem> (m_storage);
    
//     while (has_active_threads()) {
//         // TODO: implement skipping of unrelated to storage subsystem operations 

//         interleave_thread();

//         if (
//             pso_storage->has_eps_transitions(m_current_thread) &&
//             utils::get_random_in_range(0, 1) == 0
//         ) {
//             // eps transitions
//             auto location_names = pso_storage->get_propagate_locations(m_current_thread);

//             if (!location_names.empty()) {
//                 size_t index = utils::get_random_in_range(0, location_names.size() - 1);
                
//                 if (m_is_verbose) {
//                     std::cout << get_log_prefix() << "perform propagate on location '" << location_names[index] << "'";

//                     std::cout << " (all locations: ["; 
//                     for (size_t i = 0; i < location_names.size(); ++i) {
//                         std::cout << "'" << location_names[i] << "'";
//                         if (i != location_names.size() - 1) std::cout << ", ";
//                     }
//                     std::cout << "])" << std::endl;
//                 }
                
//                 pso_storage->propagate(
//                     m_current_thread,
//                     location_names[index]
//                 );
//             }
//         }
//         else {
//             // non-eps transitions
//             int instruction = m_threads[m_current_thread].instruction_index;
//             m_root->get_statements()[instruction]->accept(this);
//         }

//         if (m_is_verbose) {
//             std::cout << m_storage->get_printable_state() << std::endl;
            
//             if (m_threads.count(m_current_thread)) {
//                 std::cout << "Registers:" << std::endl << m_threads[m_current_thread].thread_subsystem.get_printable_state() << std::endl;
//             }
//         }
//     }

//     pso_storage->flush_all_buffers();

//     return get_state();
// }



// int Interpreter::pick_random_thread() const {
    // int offset = utils::get_random_in_range(0, m_threads.size() - 1);
    // // std::cout << "offset: " << offset << std::endl;
    
    // auto it = m_threads.begin();
    // std::advance(it, offset);
    // return it->first;
// }


// bool Interpreter::has_active_threads() const {
//     return !m_threads.empty();
// }


// void Interpreter::interleave_thread() {
//     // pick current thread
//     int prev_thread_id = m_current_thread;
//     m_current_thread = pick_random_thread();

//     if (prev_thread_id != m_current_thread && m_is_verbose) {
//         std::cout << "Thread interleaving: " << prev_thread_id << " -> " << m_current_thread << std::endl << std::endl;
//     }
// }


// std::string Interpreter::get_log_prefix() {
//     return "t " + std::to_string(m_current_thread) + "> ";
// }


// std::pair<
//     std::shared_ptr<StorageSubsystem>,
//     std::map<int, ThreadSubsystem>
// > Interpreter::get_state() {
//     return {
//         m_storage,
//         m_finished_thread_states
//     };
// }


void Interpreter::visit(const AstNode*, ProgramState*) {
    std::cout << "Unknown node interpreted" << std::endl;
}


void Interpreter::visit(const StatementNode* node, ProgramState* state) {
    std::cout << "Interpret StatementNode" << std::endl;

    // if (m_is_verbose) {
    //     std::cout << get_log_prefix() << utils::NodeStringifier().stringify(node) << std::endl << std::endl;
    // }
    
    node->get_statement()->accept(this, state); // inner nodes will set `state->goto_instruction` instruction index if required
    state->update_current_thread_instruction();
}


void Interpreter::visit(const GotoNode* node, ProgramState* state) {
    std::cout << "Interpret GotoNode" << std::endl;
    const auto goto_label = node->get_goto_label();

    if (!m_labeled_instructions.count(goto_label)) {
        throw exceptions::unknown_label(std::string(goto_label));
    }

    state->goto_instruction = m_labeled_instructions[goto_label];
}


void Interpreter::visit(const ThreadGotoNode* node, ProgramState* state) {
    std::cout << "Interpret ThreadGotoNode" << std::endl;
    const auto thread_start_label = node->get_thread_start_label();
    
    if (!m_labeled_instructions.count(thread_start_label)) {
        throw exceptions::unknown_label(std::string(thread_start_label));
    }
    
    // spawn new thread, starting on instruction `thread_start_label`
    state->add_thread(m_labeled_instructions[thread_start_label]);
}


void Interpreter::visit(const AssignmentNode* node, ProgramState* state) {
    std::cout << "Interpret AssignmentNode" << std::endl;
    node->get_expression()->accept(this, state); // evaluate expression to state.last_evaluated_value
    
    state->update_current_thread_register(node->get_register_name());
    // state->threads[state->current_thread_id].thread_subsystem.set(
    //     node->get_register_name(),
    //     state->last_evaluated_value
    // );
}


void Interpreter::visit(const NumberNode* node, ProgramState* state) {
    std::cout << "Interpret NumberNode" << std::endl;
    state->last_evaluated_value = node->get_value();
}


void Interpreter::visit(const BinOpNode* node, ProgramState* state) {
    std::cout << "Interpret BinOpNode" << std::endl;
    auto& thread_subsystem = state->threads[state->current_thread_id].thread_subsystem;
    
    int left = thread_subsystem.get(node->get_left_register());
    int right = thread_subsystem.get(node->get_right_register());
    BinOpNode::Type operation = node->get_operation_type();

    switch (operation) {
        case BinOpNode::Type::PLUS: {
            state->last_evaluated_value = left + right;
            break;
        }
        case BinOpNode::Type::MINUS: {
            state->last_evaluated_value = left - right;
            break;
        }
        case BinOpNode::Type::MULT: {
            state->last_evaluated_value = left * right;
            break;
        }
        case BinOpNode::Type::DIV: {
            state->last_evaluated_value = left / right;
            break;
        }
    }
}


void Interpreter::visit(const ConditionNode* node, ProgramState* state) {
    std::cout << "Interpret ConditionNode" << std::endl;
    
    auto& thread_subsystem = state->threads[state->current_thread_id].thread_subsystem;

    if (thread_subsystem.get(node->get_register_name())) {
        std::string_view goto_label = node->get_goto_label();
        
        if (!m_labeled_instructions.count(goto_label)) {
            throw exceptions::unknown_label(std::string(goto_label));
        }

        state->goto_instruction = m_labeled_instructions[goto_label];
    }
}


void Interpreter::visit(const LoadNode* node, ProgramState* state) {
    std::cout << "Interpret LoadNode" << std::endl;

    int value = state->storage->read(
        state->current_thread_id,
        node->get_location_name(),
        node->get_memory_order()
    );

    state->threads[state->current_thread_id].thread_subsystem.set(
        node->get_register_name(),
        value
    );
}


void Interpreter::visit(const StoreNode* node, ProgramState* state) {
    std::cout << "Interpret StoreNode" << std::endl;
    int value = state->threads[state->current_thread_id].thread_subsystem.get(
        node->get_register_name()
    );

    state->storage->write(
        state->current_thread_id,
        node->get_location_name(),
        value,
        node->get_memory_order()
    );
}


void Interpreter::visit(const FenceNode* node, ProgramState* state) {
    std::cout << "Interpret FenceNode" << std::endl;

    state->storage->fence(
        state->current_thread_id,
        node->get_memory_order()
    );
}


void Interpreter::visit(const CasNode* node, ProgramState* state) {
    std::cout << "Interpret CasNode" << std::endl;

    auto location_name = node->get_location_name();
    auto memory_order = node->get_memory_order();
    int current_thread_id = state->current_thread_id;

    auto& thread_subsystem = state->threads[current_thread_id].thread_subsystem;

    int expected = thread_subsystem.get(node->get_expected_register());
    int desired = thread_subsystem.get(node->get_desired_register());

    int actual = state->storage->read(
        current_thread_id,
        location_name,
        memory_order
    );

    if (actual == expected) {
        state->storage->write(
            current_thread_id,
            location_name,
            desired,
            memory_order
        );
    }

    state->last_evaluated_value = actual;
}

void Interpreter::visit(const FaiNode* node, ProgramState* state) {
    std::cout << "Interpret FaiNode" << std::endl;
    
    auto register_name = node->get_register_name();
    auto location_name = node->get_location_name();
    auto memory_order = node->get_memory_order();
    int current_thread_id = state->current_thread_id;

    int increment = state->threads[current_thread_id].thread_subsystem.get(register_name);
    
    int previous_value = state->storage->read(
        current_thread_id,
        location_name,
        memory_order
    );

    state->storage->write(
        current_thread_id,
        location_name,
        previous_value + increment,
        node->get_memory_order()
    );

    state->last_evaluated_value = previous_value;
}

void Interpreter::visit(const EndNode*, ProgramState* state) {
    std::cout << "Interpret EndNode" << std::endl;
    
    int current_thread_id = state->current_thread_id;

    auto registers = state->threads[current_thread_id].thread_subsystem.get_registers();
    std::cout << "Thread ID: " << current_thread_id << std::endl;
    for (auto& [ register_name, val ] : registers) {
        std::cout << register_name << ": " << val << std::endl;
    }
    std::cout << std::endl;
    
    state->finished_thread_states[current_thread_id] = state->threads[current_thread_id].thread_subsystem;

    state->threads.erase(current_thread_id);
}

// Explicit template instantiation
template void Interpreter::run<SCStorageSubsystem>();
// template class Interpreter<SCStorageSubsystem>;
// template class Interpreter<TSOStorageSubsystem>;
// template class Interpreter<PSOStorageSubsystem>;

}