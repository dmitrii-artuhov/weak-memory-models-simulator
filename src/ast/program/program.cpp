#include "program.h"

namespace wmm_simulator {

// ProgramNode
ProgramNode::ProgramNode(
    std::vector<std::shared_ptr<StatementNode>> statements
): m_statements(std::move(statements)) {}

void ProgramNode::accept(Visitor* visitor) const {
    visitor->visit(this);
}

const std::vector <std::shared_ptr<StatementNode>>& ProgramNode::get_statements() const {
    return m_statements;
}

}