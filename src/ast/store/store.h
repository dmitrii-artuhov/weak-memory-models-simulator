#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"
#include "lang/memory-order.h"

namespace wmm_simulator {

class StoreNode final : public AstNode {
public:
    StoreNode(MemoryOrder memory_order, std::string location_name, std::string register_name);
    void accept(Visitor* visitor) const override;
    ~StoreNode() {
        // TODO: remove
        std::cout << "~StoreNode()" << std::endl;
    }

private:
    MemoryOrder m_memory_order;
    std::string m_location_name;
    std::string m_register_name;
};


}