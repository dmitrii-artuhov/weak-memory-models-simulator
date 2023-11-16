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

    MemoryOrder get_memory_order() const;

private:
    MemoryOrder m_memory_order;
};


}