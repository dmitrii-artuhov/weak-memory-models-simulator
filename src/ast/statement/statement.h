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
    ~StatementNode() {
        // TODO: remove
        std::cout << "~StatementNode()" << std::endl;
    }

    const std::string_view get_label() const;

private:
    std::shared_ptr<AstNode> m_statement;
    std::string m_label;
};

}