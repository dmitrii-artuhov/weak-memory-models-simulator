#include "node-stringifier.h"

namespace wmm_simulator::utils {


std::string NodeStringifier::stringify(const AstNode* node) {
    m_stream.str("");

    if (node != nullptr) {
        node->accept(this, nullptr);
    }

    return m_stream.str();
}

void NodeStringifier::visit(const StatementNode* node, ProgramState*) {
    if (!node->get_label().empty()) {
        m_stream << node->get_label() << ": ";
    }

    node->get_statement()->accept(this, nullptr);
}

void NodeStringifier::visit(const GotoNode* node, ProgramState*) {
    m_stream
        << Keyword::GOTO << " "
        << node->get_goto_label();
}

void NodeStringifier::visit(const ThreadGotoNode* node, ProgramState*) {
    m_stream
        << Keyword::THREAD_GOTO << " "
        << node->get_thread_start_label();
}

void NodeStringifier::visit(const AssignmentNode* node, ProgramState*) {
    m_stream
        << node->get_register_name()
        << " = ";
    
    node->get_expression()->accept(this, nullptr);
}

void NodeStringifier::visit(const NumberNode* node, ProgramState*) {
    m_stream << node->get_value();
}

void NodeStringifier::visit(const BinOpNode* node, ProgramState*) {
    m_stream
        << node->get_left_register() << " "
        << get_binop(node->get_operation_type()) << " "
        << node->get_right_register(); 
}

void NodeStringifier::visit(const ConditionNode* node, ProgramState*) {
    m_stream
        << Keyword::CONDITION<< " "
        << node->get_register_name() << " "
        << Keyword::GOTO << " "
        << node->get_goto_label();
}

void NodeStringifier::visit(const LoadNode* node, ProgramState*) {
    m_stream
        << Keyword::LOAD << " "
        << get_memory_order(node->get_memory_order()) << " "
        << "#" << node->get_location_name() << " "
        << node->get_register_name();
}

void NodeStringifier::visit(const StoreNode* node, ProgramState*) {
    m_stream
        << Keyword::STORE << " "
        << get_memory_order(node->get_memory_order()) << " "
        << "#" << node->get_location_name() << " "
        << node->get_register_name();
}

void NodeStringifier::visit(const CasNode* node, ProgramState*) {
    m_stream
        << Keyword::CAS << " "
        << get_memory_order(node->get_memory_order()) << " "
        << "#" << node->get_location_name() << " "
        << node->get_expected_register() << " "
        << node->get_desired_register();
}

void NodeStringifier::visit(const FaiNode* node, ProgramState*) {
    m_stream
        << Keyword::FAI << " "
        << get_memory_order(node->get_memory_order()) << " "
        << "#" << node->get_location_name() << " "
        << node->get_register_name();
}

void NodeStringifier::visit(const FenceNode* node, ProgramState*) {
    m_stream
        << Keyword::FENCE << " "
        << get_memory_order(node->get_memory_order());
}

void NodeStringifier::visit(const EndNode*, ProgramState*) {
    m_stream << "end";
}


std::string NodeStringifier::get_binop(BinOpNode::Type type) const {
    switch (type) {
        case BinOpNode::Type::PLUS: return "+";
        case BinOpNode::Type::MINUS: return "-";
        case BinOpNode::Type::MULT: return "*";
        case BinOpNode::Type::DIV: return "/";
    }

    return "?";
}

std::string NodeStringifier::get_memory_order(MemoryOrder order) const {
    switch (order) {
        case MemoryOrder::SEQUENTIALLY_CONSISTENT: return Keyword::SEQ_CST;
        case MemoryOrder::ACQUIRE: return Keyword::ACQ;
        case MemoryOrder::RELEASE: return Keyword::REL;
        case MemoryOrder::RELEASE_ACQUIRE: return Keyword::REL_ACQ;
        case MemoryOrder::RELAXED: return Keyword::RLX;
    }

    return "?";
}

}