#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class EndNode final : public AstNode {
public:
    EndNode();
    void accept(Visitor* visitor, ProgramState* state) const override;
};

}