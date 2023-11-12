#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class GotoNode final : public AstNode {
public:
    GotoNode(std::string goto_label);
    void accept(const Visitor* visitor) const override;
    ~GotoNode() {
        // TODO: remove
        std::cout << "~GotoNode()" << std::endl;
    }
private:
    std::string m_goto_label;
};

}