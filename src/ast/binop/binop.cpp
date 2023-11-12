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

void BinOpNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "BinOpNode visit" << std::endl;
}

}