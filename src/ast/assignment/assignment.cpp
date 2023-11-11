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

void AssignmentNode::accept() {
    std::cout << "AssignmentNode visit" << std::endl;
}

}