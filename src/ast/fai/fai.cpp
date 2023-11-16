#include "fai.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

FaiNode::FaiNode(
    MemoryOrder memory_order,
    std::string location_name,
    std::string register_name
): m_memory_order(memory_order), m_location_name(location_name), m_register_name(register_name) {}

void FaiNode::accept(Visitor* visitor) const {
    visitor->visit(this);
}

MemoryOrder FaiNode::get_memory_order() const {
    return m_memory_order;
}

const std::string_view FaiNode::get_location_name() const {
    return std::string_view(m_location_name);
}

const std::string_view FaiNode::get_register_name() const {
    return std::string_view(m_register_name);
}

}