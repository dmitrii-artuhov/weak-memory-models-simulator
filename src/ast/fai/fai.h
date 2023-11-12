#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"
#include "lang/memory-order.h"

namespace wmm_simulator {

class FaiNode final : public AstNode {
public:
    FaiNode(MemoryOrder memory_order, std::string location_name, std::string register_name);
    void accept(const Visitor* visitor) const override;
    ~FaiNode() {
        // TODO: remove
        std::cout << "~FaiNode()" << std::endl;
    }

private:
    MemoryOrder m_memory_order;
    std::string m_location_name;
    std::string m_register_name;
};


}