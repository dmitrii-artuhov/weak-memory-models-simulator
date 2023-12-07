#include "interpreter.h"

#include <memory>
#include <iterator>

#include "state/program-state.h"
#include "utils/utils.h"
#include "exceptions/exceptions.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"
#include "ast/node.h"

namespace wmm_simulator {


Interpreter::Interpreter(
    std::shared_ptr<ProgramNode> root,
    std::unordered_map<std::string_view, int> labeled_instructions
): m_root(root), m_labeled_instructions(std::move(labeled_instructions)) {}

const ProgramNode* Interpreter::get_ast() const {
    return m_root.get();
}

int Interpreter::get_random_active_thread_id(const ProgramState& state) const {    
    int offset = utils::get_random_in_range(0, state.threads.size() - 1);

    auto it = state.threads.begin();
    std::advance(it, offset);
    
    return it->first;
}

void Interpreter::print_state(const ProgramState& state) const {
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
}

void Interpreter::visit(const AstNode*, ProgramState*) {
    // std::cout << "Unknown node interpreted" << std::endl;
}


void Interpreter::visit(const StatementNode* node, ProgramState* state) {
    // std::cout << "Interpret StatementNode" << std::endl;
    
    node->get_statement()->accept(this, state); // inner nodes will set `state->goto_instruction` instruction index if required
    state->update_current_thread_instruction();
}


void Interpreter::visit(const GotoNode* node, ProgramState* state) {
    // std::cout << "Interpret GotoNode" << std::endl;
    const auto goto_label = node->get_goto_label();

    if (!m_labeled_instructions.count(goto_label)) {
        throw exceptions::unknown_label(std::string(goto_label));
    }

    state->goto_instruction = m_labeled_instructions[goto_label];
}


void Interpreter::visit(const ThreadGotoNode* node, ProgramState* state) {
    // std::cout << "Interpret ThreadGotoNode" << std::endl;
    const auto thread_start_label = node->get_thread_start_label();
    
    if (!m_labeled_instructions.count(thread_start_label)) {
        throw exceptions::unknown_label(std::string(thread_start_label));
    }
    
    // spawn new thread, starting on instruction `thread_start_label`
    state->add_thread(m_labeled_instructions[thread_start_label]);
}


void Interpreter::visit(const AssignmentNode* node, ProgramState* state) {
    // std::cout << "Interpret AssignmentNode" << std::endl;
    node->get_expression()->accept(this, state); // evaluate expression to state.last_evaluated_value
    
    state->update_current_thread_register(node->get_register_name());
}


void Interpreter::visit(const NumberNode* node, ProgramState* state) {
    // std::cout << "Interpret NumberNode" << std::endl;
    state->last_evaluated_value = node->get_value();
}


void Interpreter::visit(const BinOpNode* node, ProgramState* state) {
    // std::cout << "Interpret BinOpNode" << std::endl;
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
    // std::cout << "Interpret ConditionNode" << std::endl;
    
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
    // std::cout << "Interpret LoadNode" << std::endl;

    int value = state->storage->read(
        state->current_thread_id,
        node->get_location_name().data(),
        node->get_memory_order()
    );

    state->threads[state->current_thread_id].thread_subsystem.set(
        node->get_register_name(),
        value
    );
}


void Interpreter::visit(const StoreNode* node, ProgramState* state) {
    // std::cout << "Interpret StoreNode" << std::endl;
    int value = state->threads[state->current_thread_id].thread_subsystem.get(
        node->get_register_name()
    );

    state->storage->write(
        state->current_thread_id,
        node->get_location_name().data(),
        value,
        node->get_memory_order()
    );
}


void Interpreter::visit(const FenceNode* node, ProgramState* state) {
    // std::cout << "Interpret FenceNode" << std::endl;

    state->storage->fence(
        state->current_thread_id,
        node->get_memory_order()
    );
}

void Interpreter::visit(const CasNode* node, ProgramState* state) {
    // std::cout << "Interpret CasNode" << std::endl;

    auto location_name = node->get_location_name().data();
    auto memory_order = node->get_memory_order();
    int current_thread_id = state->current_thread_id;

    auto& thread_subsystem = state->threads[current_thread_id].thread_subsystem;

    int expected = thread_subsystem.get(node->get_expected_register());
    int desired = thread_subsystem.get(node->get_desired_register());

    state->storage->flush();

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
    // std::cout << "Interpret FaiNode" << std::endl;
    
    auto register_name = node->get_register_name();
    auto location_name = node->get_location_name().data();
    auto memory_order = node->get_memory_order();
    int current_thread_id = state->current_thread_id;

    int increment = state->threads[current_thread_id].thread_subsystem.get(register_name);
    
    state->storage->flush();
    
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
    // std::cout << "Interpret EndNode" << std::endl;
    
    int current_thread_id = state->current_thread_id;    
    state->finished_thread_states[current_thread_id] = state->threads[current_thread_id].thread_subsystem;

    state->threads.erase(current_thread_id);
}

}