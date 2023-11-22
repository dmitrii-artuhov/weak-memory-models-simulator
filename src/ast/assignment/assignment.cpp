#include "assignment.h"

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"

namespace wmm_simulator {

// AssignmentNode
AssignmentNode::AssignmentNode(
    std::string register_name,
    std::shared_ptr<AstNode> expr
): m_register_name(std::move(register_name)), m_expr(expr) {}

void AssignmentNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

const std::string_view AssignmentNode::get_register_name() const {
    return std::string_view(m_register_name);
}

const std::shared_ptr<AstNode> AssignmentNode::get_expression() const {
    return m_expr;
}

}