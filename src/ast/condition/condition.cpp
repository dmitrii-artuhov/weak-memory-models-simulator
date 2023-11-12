#include "condition.h"


namespace wmm_simulator {

// ConditionNode
ConditionNode::ConditionNode(
    std::string register_node,
    std::string goto_label
): m_register_name(std::move(register_node)), m_goto_label(std::move(goto_label)) {}

void ConditionNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "ConditionNode visist" << std::endl;
}

}