#include "cas.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

CasNode::CasNode(
    MemoryOrder memory_order,
    std::string location_name,
    std::string register_expected,
    std::string register_desired
): m_memory_order(memory_order), m_location_name(location_name), m_register_expected(register_expected), m_register_desired(register_desired) {}

void CasNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

MemoryOrder CasNode::get_memory_order() const {
    return m_memory_order;
}

const std::string_view CasNode::get_location_name() const {
    return std::string_view(m_location_name);
}

const std::string_view CasNode::get_expected_register() const {
    return std::string_view(m_register_expected);
}

const std::string_view CasNode::get_desired_register() const {
    return std::string_view(m_register_desired);
}

}