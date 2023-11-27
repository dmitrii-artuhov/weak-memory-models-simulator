#include "condition.h"


namespace wmm_simulator {

// ConditionNode
ConditionNode::ConditionNode(
    std::string register_node,
    std::string goto_label
): m_register_name(std::move(register_node)), m_goto_label(std::move(goto_label)) {}

void ConditionNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

const std::string_view ConditionNode::get_register_name() const {
    return std::string_view(m_register_name);
}

const std::string_view ConditionNode::get_goto_label() const {
    return std::string_view(m_goto_label);
}

}