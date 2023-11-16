#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class ConditionNode final : public AstNode {
public:
    ConditionNode(std::string register_name, std::string goto_label);
    void accept(Visitor* visitor) const override;

    const std::string_view get_register_name() const;
    const std::string_view get_goto_label() const;

private:
    std::string m_register_name;
    std::string m_goto_label;
};

}