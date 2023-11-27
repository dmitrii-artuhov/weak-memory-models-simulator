#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class GotoNode final : public AstNode {
public:
    GotoNode(std::string goto_label);
    void accept(Visitor* visitor, ProgramState* state) const override;
    
    const std::string_view get_goto_label() const;
private:
    std::string m_goto_label;
};

}