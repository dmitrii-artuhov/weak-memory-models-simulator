#include "fence.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

FenceNode::FenceNode(
    MemoryOrder memory_order
): m_memory_order(memory_order) {}

void FenceNode::accept(Visitor* visitor) const {
    visitor->visit(this);
}

}