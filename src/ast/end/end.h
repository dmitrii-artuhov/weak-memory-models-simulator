#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class EndNode final : public AstNode {
public:
    EndNode();
    void accept() override;
    ~EndNode() {
        // TODO: remove
        std::cout << "~EndNode()" << std::endl;
    }
};

}