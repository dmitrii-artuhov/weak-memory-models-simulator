#include "node-evaluator.h"

namespace wmm_simulator::utils {


bool NodeEvaluator::is_allowed_interleaving(const AstNode* node) {
    m_is_allowed = false;

    if (node != nullptr) {
        node->accept(this, nullptr);
    }

    return m_is_allowed;
}

void NodeEvaluator::visit(const StatementNode* node, ProgramState*) {
    node->get_statement()->accept(this, nullptr);
}

void NodeEvaluator::visit(const AssignmentNode* node, ProgramState*) {
    node->get_expression()->accept(this, nullptr);
}

void NodeEvaluator::visit(const LoadNode* node, ProgramState*) {
    m_is_allowed = true;
}

void NodeEvaluator::visit(const StoreNode* node, ProgramState*) {
    m_is_allowed = true;
}

void NodeEvaluator::visit(const CasNode* node, ProgramState*) {
    m_is_allowed = true;
}

void NodeEvaluator::visit(const FaiNode* node, ProgramState*) {
    m_is_allowed = true;
}

void NodeEvaluator::visit(const FenceNode* node, ProgramState*) {
    m_is_allowed = true;
}

void NodeEvaluator::visit(const EndNode*, ProgramState*) {
    m_is_allowed = true;
}

}