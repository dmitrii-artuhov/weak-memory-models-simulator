#include "program.h"

namespace wmm_simulator {

// ProgramNode
ProgramNode::ProgramNode(
    std::vector<std::shared_ptr<StatementNode>> statements
): m_statements(std::move(statements)) {}

void ProgramNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "ProgramNode visit" << std::endl;
}

const std::vector <std::shared_ptr<StatementNode>>& ProgramNode::get_statements() const {
    return m_statements;
}

}