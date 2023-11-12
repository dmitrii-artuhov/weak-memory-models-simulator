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

void CasNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "CasNode visit" << std::endl;
}

}