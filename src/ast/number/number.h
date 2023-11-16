#pragma once

#include <iostream>

#include "ast/node.h"

namespace wmm_simulator {

class NumberNode final : public AstNode {
public:
    NumberNode(int value);
    void accept(Visitor* visitor) const override;

    int get_value() const;

private:
    int m_value;
};

}