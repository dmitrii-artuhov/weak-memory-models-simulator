#include "program.h"

namespace wmm_simulator {

// ProgramNode
ProgramNode::ProgramNode(
    std::vector<std::shared_ptr<StatementNode>> statements
): m_statements(std::move(statements)) {}

void ProgramNode::accept() {
    std::cout << "ProgramNode visit" << std::endl;
}

}