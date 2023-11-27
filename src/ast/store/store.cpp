#include "store.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

StoreNode::StoreNode(
    MemoryOrder memory_order,
    std::string location_name,
    std::string register_name
): m_memory_order(memory_order), m_location_name(location_name), m_register_name(register_name) {}

void StoreNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

MemoryOrder StoreNode::get_memory_order() const {
    return m_memory_order;
}

const std::string_view StoreNode::get_location_name() const {
    return std::string_view(m_location_name);
}

const std::string_view StoreNode::get_register_name() const {
    return std::string_view(m_register_name);
}

}