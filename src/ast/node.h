#pragma once

#include <vector>
#include <string>
#include <memory>

#include "visitor/visitor.h"

namespace wmm_simulator {

class AstNode {
public:
    virtual void accept(const Visitor* visitor) const = 0;
    virtual ~AstNode() {}
};

}