#include "binop.h"

#include <vector>
#include <string>

namespace wmm_simulator {

// BinOpNode
BinOpNode::BinOpNode(
    std::string left_register,
    Type operation,
    std::string right_register
): m_left_register(std::move(left_register)), m_operation(operation), m_right_register(std::move(right_register)) {}

void BinOpNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

const std::string_view BinOpNode::get_left_register() const {
    return std::string_view(m_left_register);
}

const std::string_view BinOpNode::get_right_register() const {
    return std::string_view(m_right_register);
}

BinOpNode::Type BinOpNode::get_operation_type() const {
    return m_operation;
}

}