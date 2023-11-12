#include "number.h"

#include <iostream>

namespace wmm_simulator {

// NumberNode
NumberNode::NumberNode(int value): m_value(value) {}

void NumberNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "NumberNode visit" << std::endl;
}

}