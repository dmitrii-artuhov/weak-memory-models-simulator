#include "load.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

LoadNode::LoadNode(
    MemoryOrder memory_order,
    std::string location_name,
    std::string register_name
): m_memory_order(memory_order), m_location_name(location_name), m_register_name(register_name) {}

void LoadNode::accept(Visitor* visitor) const {
    visitor->visit(this);
}

MemoryOrder LoadNode::get_memory_order() const {
    return m_memory_order;
}

const std::string_view LoadNode::get_location_name() const {
    return std::string_view(m_location_name);
}

const std::string_view LoadNode::get_register_name() const {
    return std::string_view(m_register_name);
}

}