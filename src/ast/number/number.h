#pragma once

#include <iostream>

#include "ast/node.h"

namespace wmm_simulator {

class NumberNode final : public AstNode {
public:
    NumberNode(int value);
    void accept(const Visitor* visitor) const override;
    ~NumberNode() {
        // TODO: remove
        std::cout << "~NumberNode()" << std::endl;
    }

private:
    int m_value;
};

}