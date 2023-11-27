#include "statement.h"

#include <iostream>
#include <memory>

namespace wmm_simulator {

StatementNode::StatementNode(std::shared_ptr<AstNode> statement, std::string label): m_statement(statement), m_label(std::move(label)) {}

void StatementNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

const std::string_view StatementNode::get_label() const {
    return m_label;
}

const std::shared_ptr<AstNode> StatementNode::get_statement() const {
    return m_statement;
}

}