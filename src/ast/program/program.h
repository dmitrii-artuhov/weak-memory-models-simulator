#pragma once
#include "program.h"

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"

namespace wmm_simulator {

class ProgramNode final : public AstNode {
public:
    ProgramNode(std::vector<std::shared_ptr<AstNode>>&& statements);
    void accept() override;
    ~ProgramNode() {
        // TODO: remove
        std::cout << "~ProgramNode()" << std::endl;
    }

private:
    std::vector <std::shared_ptr<AstNode>> m_statements;
};


}