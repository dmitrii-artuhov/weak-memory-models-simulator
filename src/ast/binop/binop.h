#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class BinOpNode final : public AstNode {
public:
    enum class Type {
        PLUS,
        MINUS,
        MULT,
        DIV
    };

    BinOpNode(std::string left_register, Type operation, std::string right_register);
    void accept(Visitor* visitor, ProgramState* state) const override;

    const std::string_view get_left_register() const;
    const std::string_view get_right_register() const;
    Type get_operation_type() const;

private:
    std::string m_left_register;
    Type m_operation;
    std::string m_right_register;
};

}