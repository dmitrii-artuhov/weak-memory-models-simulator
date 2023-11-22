#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"
#include "ast/statement/statement.h"

namespace wmm_simulator {

class ProgramNode final : public AstNode {
public:
    ProgramNode(std::vector<std::shared_ptr<StatementNode>> statements);
    void accept(Visitor* visitor, ProgramState* state) const override;

    const std::vector<std::shared_ptr<StatementNode>>& get_statements() const;

private:
    std::vector <std::shared_ptr<StatementNode>> m_statements;
};


}