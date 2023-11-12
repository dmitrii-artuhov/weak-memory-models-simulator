#include "fai.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

FaiNode::FaiNode(
    MemoryOrder memory_order,
    std::string location_name,
    std::string register_name
): m_memory_order(memory_order), m_location_name(location_name), m_register_name(register_name) {}

void FaiNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "FaiNode visit" << std::endl;
}

}