#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"
#include "lang/memory-order.h"

namespace wmm_simulator {

class LoadNode final : public AstNode {
public:
    LoadNode(MemoryOrder memory_order, std::string location_name, std::string register_name);
    void accept() override;
    ~LoadNode() {
        // TODO: remove
        std::cout << "~LoadNode()" << std::endl;
    }

private:
    MemoryOrder m_memory_order;
    std::string m_location_name;
    std::string m_register_name;
};


}