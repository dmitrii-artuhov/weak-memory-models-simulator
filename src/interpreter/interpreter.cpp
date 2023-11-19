#include "interpreter.h"

#include <memory>
#include <iterator>

#include "utils/utils.h"
#include "exceptions/exceptions.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"
#include "ast/node.h"
#include "utils/node-stringifier.h"


namespace wmm_simulator {

template<class T>
Interpreter<T>::Interpreter(
    std::shared_ptr<ProgramNode> root,
    std::unordered_map<std::string_view, int> labeled_instructions,
    bool is_verbose
): m_root(root), m_labeled_instructions(std::move(labeled_instructions)), m_is_verbose(is_verbose) {}

template<class T>
void Interpreter<T>::init() {
    m_max_thread_index = 0;
    m_current_thread = 0;
    m_last_evaluated_value = 0;
    m_goto_instruction = -1;
    
    m_threads.clear();
    m_finished_thread_states.clear();
    
    m_storage = std::shared_ptr<StorageSubsystem>(new T());
    // Spawn main thread
    m_threads[0] = {
        0, // since END instuction is always in the ast
        ThreadSubsystem()
    };
}

/*  Specialization for a SC storage subsystem */
template<>
std::pair<
    std::shared_ptr<StorageSubsystem>,
    std::map<int, ThreadSubsystem>
> Interpreter<SCStorageSubsystem>::run() {
    init();

    while (has_active_threads()) {
        // TODO: implement skipping of unrelated to storage subsystem operations 

        interleave_thread();

        // non-eps transitions
        int instruction = m_threads[m_current_thread].instruction_index;
        m_root->get_statements()[instruction]->accept(this);

        if (m_is_verbose) {
            std::cout << m_storage->get_printable_state() << std::endl;
            
            if (m_threads.count(m_current_thread)) {
                std::cout << "Registers:" << std::endl << m_threads[m_current_thread].thread_subsystem.get_printable_state() << std::endl;
            }
        }
    }

    return get_state();
}


/*  Specialization for a TSO storage subsystem */
template<>
std::pair<
    std::shared_ptr<StorageSubsystem>,
    std::map<int, ThreadSubsystem>
> Interpreter<TSOStorageSubsystem>::run() {
    init();

    std::shared_ptr<TSOStorageSubsystem> tso_storage = std::static_pointer_cast<TSOStorageSubsystem> (m_storage);
    
    while (has_active_threads()) {
        // TODO: implement skipping of unrelated to storage subsystem operations 

        interleave_thread();

        if (
            tso_storage->has_eps_transitions(m_current_thread) &&
            utils::get_random_in_range(0, 1) == 0
        ) {
            // eps transitions
            if (m_is_verbose) {
                std::cout << get_log_prefix() << "perform propagate" << std::endl << std::endl;
            }
            tso_storage->propagate(m_current_thread);
        }
        else {
            // non-eps transitions
            int instruction = m_threads[m_current_thread].instruction_index;
            m_root->get_statements()[instruction]->accept(this);
        }

        if (m_is_verbose) {
            std::cout << m_storage->get_printable_state() << std::endl;
            
            if (m_threads.count(m_current_thread)) {
                std::cout << "Registers:" << std::endl << m_threads[m_current_thread].thread_subsystem.get_printable_state() << std::endl;
            }
        }
    }

    tso_storage->flush_all_buffers();

    return get_state();
}


/*  Specialization for a PSO storage subsystem */
template<>
std::pair<
    std::shared_ptr<StorageSubsystem>,
    std::map<int, ThreadSubsystem>
> Interpreter<PSOStorageSubsystem>::run() {
    init();

    std::shared_ptr<PSOStorageSubsystem> pso_storage = std::static_pointer_cast<PSOStorageSubsystem> (m_storage);
    
    while (has_active_threads()) {
        // TODO: implement skipping of unrelated to storage subsystem operations 

        interleave_thread();

        if (
            pso_storage->has_eps_transitions(m_current_thread) &&
            utils::get_random_in_range(0, 1) == 0
        ) {
            // eps transitions
            auto location_names = pso_storage->get_propagate_locations(m_current_thread);

            if (!location_names.empty()) {
                size_t index = utils::get_random_in_range(0, location_names.size() - 1);
                
                if (m_is_verbose) {
                    std::cout << get_log_prefix() << "perform propagate on location '" << location_names[index] << "'";

                    std::cout << " (all locations: ["; 
                    for (size_t i = 0; i < location_names.size(); ++i) {
                        std::cout << "'" << location_names[i] << "'";
                        if (i != location_names.size() - 1) std::cout << ", ";
                    }
                    std::cout << "])" << std::endl;
                }
                
                pso_storage->propagate(
                    m_current_thread,
                    location_names[index]
                );
            }
        }
        else {
            // non-eps transitions
            int instruction = m_threads[m_current_thread].instruction_index;
            m_root->get_statements()[instruction]->accept(this);
        }

        if (m_is_verbose) {
            std::cout << m_storage->get_printable_state() << std::endl;
            
            if (m_threads.count(m_current_thread)) {
                std::cout << "Registers:" << std::endl << m_threads[m_current_thread].thread_subsystem.get_printable_state() << std::endl;
            }
        }
    }

    pso_storage->flush_all_buffers();

    return get_state();
}


template<class T>
int Interpreter<T>::pick_random_thread() const {
    int offset = utils::get_random_in_range(0, m_threads.size() - 1);
    // std::cout << "offset: " << offset << std::endl;
    
    auto it = m_threads.begin();
    std::advance(it, offset);
    return it->first;
}

template<class T>
bool Interpreter<T>::has_active_threads() const {
    return !m_threads.empty();
}

template<class T>
void Interpreter<T>::interleave_thread() {
    // pick current thread
    int prev_thread_id = m_current_thread;
    m_current_thread = pick_random_thread();

    if (prev_thread_id != m_current_thread && m_is_verbose) {
        std::cout << "Thread interleaving: " << prev_thread_id << " -> " << m_current_thread << std::endl << std::endl;
    }
}

template<class T>
std::string Interpreter<T>::get_log_prefix() {
    return "t " + std::to_string(m_current_thread) + "> ";
}

template<class T>
std::pair<
    std::shared_ptr<StorageSubsystem>,
    std::map<int, ThreadSubsystem>
> Interpreter<T>::get_state() {
    return {
        m_storage,
        m_finished_thread_states
    };
}

template<class T>
void Interpreter<T>::visit(const AstNode*) {
    std::cout << "Unknown node interpreted" << std::endl;
}

template<class T>
void Interpreter<T>::visit(const StatementNode* node) {
    // std::cout << "Interpret StatementNode" << std::endl;

    if (m_is_verbose) {
        std::cout << get_log_prefix() << utils::NodeStringifier().stringify(node) << std::endl << std::endl;
    }
    
    node->get_statement()->accept(this); // inner nodes will set `m_goto_instruction` instruction index if required

    if (m_threads.count(m_current_thread)) {
        if (m_goto_instruction == -1) {
            m_threads[m_current_thread].instruction_index++;
        }
        else {
            m_threads[m_current_thread].instruction_index = m_goto_instruction;
            m_goto_instruction = -1;
        }
    }
}

template<class T>
void Interpreter<T>::visit(const GotoNode* node) {
    // std::cout << "Interpret GotoNode" << std::endl;
    const auto goto_label = node->get_goto_label();

    if (!m_labeled_instructions.count(goto_label)) {
        throw exceptions::unknown_label(std::string(goto_label));
    }

    m_goto_instruction = m_labeled_instructions[goto_label];
}

template<class T>
void Interpreter<T>::visit(const ThreadGotoNode* node) {
    // std::cout << "Interpret ThreadGotoNode" << std::endl;
    const auto thread_start_label = node->get_thread_start_label();
    
    if (!m_labeled_instructions.count(thread_start_label)) {
        throw exceptions::unknown_label(std::string(thread_start_label));
    }
    
    // spawn new thread, starting on instruction `thread_start_label`
    m_threads[++m_max_thread_index] = {
        m_labeled_instructions[thread_start_label],
        ThreadSubsystem()
    };
}

template<class T>
void Interpreter<T>::visit(const AssignmentNode* node) {
    // std::cout << "Interpret AssignmentNode" << std::endl;
    node->get_expression()->accept(this); // evaluate expression to m_last_evaluated_value
    m_threads[m_current_thread].thread_subsystem.set(
        node->get_register_name(),
        m_last_evaluated_value
    );
}

template<class T>
void Interpreter<T>::visit(const NumberNode* node) {
    // std::cout << "Interpret NumberNode" << std::endl;
    m_last_evaluated_value = node->get_value();
}

template<class T>
void Interpreter<T>::visit(const BinOpNode* node) {
    // std::cout << "Interpret BinOpNode" << std::endl;
    auto& thread_subsystem = m_threads[m_current_thread].thread_subsystem;
    
    int left = thread_subsystem.get(node->get_left_register());
    int right = thread_subsystem.get(node->get_right_register());
    BinOpNode::Type operation = node->get_operation_type();

    switch (operation) {
        case BinOpNode::Type::PLUS: {
            m_last_evaluated_value = left + right;
            break;
        }
        case BinOpNode::Type::MINUS: {
            m_last_evaluated_value = left - right;
            break;
        }
        case BinOpNode::Type::MULT: {
            m_last_evaluated_value = left * right;
            break;
        }
        case BinOpNode::Type::DIV: {
            m_last_evaluated_value = left / right;
            break;
        }
    }
}

template<class T>
void Interpreter<T>::visit(const ConditionNode* node) {
    // std::cout << "Interpret ConditionNode" << std::endl;
    
    auto& thread_subsystem = m_threads[m_current_thread].thread_subsystem;
    if (thread_subsystem.get(node->get_register_name())) {
        std::string_view goto_label = node->get_goto_label();
        if (!m_labeled_instructions.count(goto_label)) {
            throw exceptions::unknown_label(std::string(goto_label));
        }
        m_goto_instruction = m_labeled_instructions[goto_label];
    }
}

template<class T>
void Interpreter<T>::visit(const LoadNode* node) {
    // std::cout << "Interpret LoadNode" << std::endl;

    int value = m_storage->read(
        m_current_thread,
        node->get_location_name(),
        node->get_memory_order()
    );
    m_threads[m_current_thread].thread_subsystem.set(
        node->get_register_name(),
        value
    );
}

template<class T>
void Interpreter<T>::visit(const StoreNode* node) {
    // std::cout << "Interpret StoreNode" << std::endl;
    int value = m_threads[m_current_thread].thread_subsystem.get(
        node->get_register_name()
    );
    m_storage->write(
        m_current_thread,
        node->get_location_name(),
        value,
        node->get_memory_order()
    );
}

template<class T>
void Interpreter<T>::visit(const FenceNode* node) {
    // std::cout << "Interpret FenceNode" << std::endl;

    m_storage->fence(
        m_current_thread,
        node->get_memory_order()
    );
}

template<class T>
void Interpreter<T>::visit(const CasNode* node) {
    // std::cout << "Interpret CasNode" << std::endl;

    auto location_name = node->get_location_name();
    auto memory_order = node->get_memory_order();
    auto& thread_subsystem = m_threads[m_current_thread].thread_subsystem;

    int expected = thread_subsystem.get(node->get_expected_register());
    int desired = thread_subsystem.get(node->get_desired_register());

    int actual = m_storage->read(
        m_current_thread,
        location_name,
        memory_order
    );

    if (actual == expected) {
        m_storage->write(
            m_current_thread,
            location_name,
            desired,
            memory_order
        );
    }

    m_last_evaluated_value = actual;
}

template<class T>
void Interpreter<T>::visit(const FaiNode* node) {
    // std::cout << "Interpret FaiNode" << std::endl;
    
    auto register_name = node->get_register_name();
    auto location_name = node->get_location_name();
    auto memory_order = node->get_memory_order();

    int increment = m_threads[m_current_thread].thread_subsystem.get(register_name);
    
    int previous_value = m_storage->read(
        m_current_thread,
        location_name,
        memory_order
    );

    m_storage->write(
        m_current_thread,
        location_name,
        previous_value + increment,
        node->get_memory_order()
    );

    m_last_evaluated_value = previous_value;
}

template<class T>
void Interpreter<T>::visit(const EndNode*) {
    // std::cout << "Interpret EndNode" << std::endl;
    
    // auto registers = m_threads[m_current_thread].thread_subsystem.get_registers();
    // std::cout << "Thread ID: " << m_current_thread << std::endl;
    // for (auto& [ register_name, val ] : registers) {
    //     std::cout << register_name << ": " << val << std::endl;
    // }
    // std::cout << std::endl;
    
    m_finished_thread_states[m_current_thread] = m_threads[m_current_thread].thread_subsystem;

    m_threads.erase(m_current_thread);
}

// Explicit template instantiation
template class Interpreter<SCStorageSubsystem>;
template class Interpreter<TSOStorageSubsystem>;
template class Interpreter<PSOStorageSubsystem>;

}