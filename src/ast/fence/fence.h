#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"
#include "lang/memory-order.h"

namespace wmm_simulator {

class FenceNode final : public AstNode {
public:
    FenceNode(MemoryOrder memory_order);
    void accept(Visitor* visitor) const override;
    ~FenceNode() {
        // TODO: remove
        std::cout << "~FenceNode()" << std::endl;
    }

private:
    MemoryOrder m_memory_order;
};


}