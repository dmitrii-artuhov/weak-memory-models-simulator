#pragma once

#include <vector>
#include <string>
#include <memory>

namespace wmm_simulator {

class AstNode {
public:
    virtual void accept() = 0; // TODO: implement visitor pattern here
    virtual ~AstNode() {}
};

}