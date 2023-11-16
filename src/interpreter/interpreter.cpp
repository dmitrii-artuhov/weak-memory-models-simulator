#include "interpreter.h"

#include <memory>

#include "utils/utils.h"
#include "exceptions/exceptions.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "ast/node.h"
#include "ast/program/program.h"


namespace wmm_simulator {

template<class T>
Interpreter<T>::Interpreter(
    std::shared_ptr<ProgramNode> root,
    std::unordered_map<std::string_view, int> labeled_instructions
): m_root(root), m_labeled_instructions(std::move(labeled_instructions)) {
    
    m_storage = std::shared_ptr<StorageSubsystem>(new T());

    // Start main thread
    m_threads[0] = {
        0, // since END instuction is always in the ast
        ThreadSubsystem()
    };
}

template<class T>
std::shared_ptr<StorageSubsystem> Interpreter<T>::run() {
    while (has_active_threads()) {
        // pick current thread
        m_current_thread = utils::get_random_in_range(0, m_threads.size() - 1);

        // eps transitions (for now SC model, no eps)

        // non-eps transitions
        int instruction = m_threads[m_current_thread].instruction_index;
        m_root->get_statements()[instruction]->accept(this);
    }

    return m_storage;
}

template<class T>
bool Interpreter<T>::has_active_threads() const {
    return !m_threads.empty();
}

template<class T>
void Interpreter<T>::visit(const AstNode*) {
    std::cout << "Unknown node interpreted" << std::endl;
}

template<class T>
void Interpreter<T>::visit(const StatementNode* node) {
    std::cout << "Interpret StatementNode" << std::endl;
    node->accept(this); // inner nodes will change instruction index 
}

template<class T>
void Interpreter<T>::visit(const GotoNode* node) {
    std::cout << "Interpret GotoNode" << std::endl;
    const auto goto_label = node->get_goto_label();

    if (!m_labeled_instructions.count(goto_label)) {
        throw exceptions::unknown_label(std::string(goto_label));
    }

    m_threads[m_current_thread].instruction_index = m_labeled_instructions[goto_label];
}

template<class T>
void Interpreter<T>::visit(const ThreadGotoNode* node) {
    std::cout << "Interpret ThreadGotoNode" << std::endl;
    const auto thread_start_label = node->get_thread_start_label();
    
    if (!m_labeled_instructions.count(thread_start_label)) {
        throw exceptions::unknown_label(std::string(thread_start_label));
    }
    
    // spawn new thread, starting on instruction `thread_start_label`
    m_threads[++m_max_thread_index] = {
        m_labeled_instructions[thread_start_label],
        ThreadSubsystem()
    };

    m_threads[m_current_thread].instruction_index++;
}

template<class T>
void Interpreter<T>::visit(const AssignmentNode* node) {
    std::cout << "Interpret AssignmentNode" << std::endl;
    node->get_expression()->accept(this); // evaluate expression to m_last_evaluated_value
    m_threads[m_current_thread].thread_subsystem.set(
        node->get_register_name(),
        m_last_evaluated_value
    );

    m_threads[m_current_thread].instruction_index++;
}

template<class T>
void Interpreter<T>::visit(const NumberNode* node) {
    std::cout << "Interpret NumberNode" << std::endl;
    m_last_evaluated_value = node->get_value();
}

template<class T>
void Interpreter<T>::visit(const BinOpNode* node) {
    std::cout << "Interpret BinOpNode" << std::endl;
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
    std::cout << "Interpret ConditionNode" << std::endl;
    
    auto& thread_subsystem = m_threads[m_current_thread].thread_subsystem;
    if (thread_subsystem.get(node->get_register_name())) {
        std::string_view goto_label = node->get_goto_label();
        if (!m_labeled_instructions.count(goto_label)) {
            throw exceptions::unknown_label(std::string(goto_label));
        }
        m_threads[m_current_thread].instruction_index = m_labeled_instructions[goto_label];
    }
    else {
        m_threads[m_current_thread].instruction_index++;
    }
}


template<class T>
void Interpreter<T>::visit(const LoadNode* node) {
    std::cout << "Interpret LoadNode" << std::endl;

    int value = m_storage->read(
        node->get_location_name(),
        node->get_memory_order()
    );
    m_threads[m_current_thread].thread_subsystem.set(
        node->get_register_name(),
        value
    );

    m_threads[m_current_thread].instruction_index++;
}

template<class T>
void Interpreter<T>::visit(const StoreNode*) {
    std::cout << "Interpret StoreNode" << std::endl;
    
}

template<class T>
void Interpreter<T>::visit(const EndNode*) {
    std::cout << "Interpret EndNode" << std::endl;
    m_threads.erase(m_current_thread);
}


// Explicit template instantiation
template class Interpreter<SCStorageSubsystem>;

}