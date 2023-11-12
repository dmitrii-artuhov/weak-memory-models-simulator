#include "statement.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

StatementNode::StatementNode(std::shared_ptr<AstNode> statement, std::string label): m_statement(statement), m_label(std::move(label)) {}

void StatementNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "StatementNode visit" << std::endl;
}

const std::string_view StatementNode::get_label() const {
    return m_label;
}

}