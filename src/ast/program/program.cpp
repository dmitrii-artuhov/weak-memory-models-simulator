#include "program.h"

namespace wmm_simulator {

// ProgramNode
ProgramNode::ProgramNode(
    std::vector<std::shared_ptr<StatementNode>> statements
): m_statements(std::move(statements)) {}

void ProgramNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

const std::vector <std::shared_ptr<StatementNode>>& ProgramNode::get_statements() const {
    return m_statements;
}

}