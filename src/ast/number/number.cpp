#include "number.h"

#include <iostream>

namespace wmm_simulator {

// NumberNode
NumberNode::NumberNode(int value): m_value(value) {}

void NumberNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

int NumberNode::get_value() const {
    return m_value;
}

}