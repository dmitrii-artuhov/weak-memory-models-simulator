#pragma once

#include <vector>
#include <string>
#include <memory>

#include "visitor/visitor.h"

namespace wmm_simulator {

class ProgramState;

class AstNode {
public:
    virtual void accept(Visitor* visitor, ProgramState* state) const = 0;
    virtual ~AstNode() {}
};

}