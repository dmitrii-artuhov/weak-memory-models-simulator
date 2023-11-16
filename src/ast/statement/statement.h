#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class StatementNode : public AstNode {
public:
    StatementNode(std::shared_ptr<AstNode> statement, std::string label = "");
    void accept(Visitor* visitor) const override;

    const std::string_view get_label() const;
    const std::shared_ptr<AstNode> get_statement() const;

private:
    std::shared_ptr<AstNode> m_statement;
    std::string m_label;
};

}