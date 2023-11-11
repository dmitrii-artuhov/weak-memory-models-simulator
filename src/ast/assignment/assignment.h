#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "ast/node.h"

namespace wmm_simulator {

class AssignmentNode final : public AstNode {
public:
    AssignmentNode(std::string register_name, std::shared_ptr<AstNode> expr);
    void accept() override;
    ~AssignmentNode() {
        // TODO: remove
        std::cout << "~AssignmentNode()" << std::endl;
    }

private:
    std::string m_register_name;
    std::shared_ptr<AstNode> m_expr;
};


}