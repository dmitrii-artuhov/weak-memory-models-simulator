#include "statement.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

StatementNode::StatementNode(std::shared_ptr<AstNode> statement, std::string label): m_statement(statement), m_label(std::move(label)) {}

void StatementNode::accept() {
    std::cout << "StatementNode visit" << std::endl;
}

}