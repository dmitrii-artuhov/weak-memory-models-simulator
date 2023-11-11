#include "store.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

StoreNode::StoreNode(
    MemoryOrder memory_order,
    std::string location_name,
    std::string register_name
): m_memory_order(memory_order), m_location_name(location_name), m_register_name(register_name) {}

void StoreNode::accept() {
    std::cout << "StoreNode visit" << std::endl;
}

}