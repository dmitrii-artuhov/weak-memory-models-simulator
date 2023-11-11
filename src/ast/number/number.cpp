#include "number.h"

#include <iostream>

namespace wmm_simulator {

// NumberNode
NumberNode::NumberNode(int value): m_value(value) {}

void NumberNode::accept() {
    std::cout << "NumberNode visit" << std::endl;
}

}