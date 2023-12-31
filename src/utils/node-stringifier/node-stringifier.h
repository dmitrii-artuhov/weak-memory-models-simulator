#pragma once

#include <string>
#include <sstream>

#include "visitor/visitor.h"
#include "lang/keywords.h"
#include "state/program-state.h"

#include "ast/node.h"
#include "ast/program/program.h"
#include "ast/statement/statement.h"
#include "ast/assignment/assignment.h"
#include "ast/number/number.h"
#include "ast/binop/binop.h"
#include "ast/condition/condition.h"
#include "ast/load/load.h"
#include "ast/store/store.h"
#include "ast/cas/cas.h"
#include "ast/fai/fai.h"
#include "ast/fence/fence.h"
#include "ast/goto/goto.h"
#include "ast/thread-goto/thread-goto.h"
#include "ast/end/end.h"

namespace wmm_simulator::utils {

class NodeStringifier : public Visitor {
public:
    std::string stringify(const AstNode* node);

    void visit(const StatementNode* node, ProgramState*) override;
    void visit(const GotoNode* node, ProgramState*) override;
    void visit(const ThreadGotoNode* node, ProgramState*) override;
    void visit(const AssignmentNode* node, ProgramState*) override;
    void visit(const NumberNode* node, ProgramState*) override;
    void visit(const BinOpNode* node, ProgramState*) override;
    void visit(const ConditionNode* node, ProgramState*) override;
    void visit(const LoadNode* node, ProgramState*) override;
    void visit(const StoreNode* node, ProgramState*) override;
    void visit(const CasNode* node, ProgramState*) override;
    void visit(const FaiNode* node, ProgramState*) override;
    void visit(const FenceNode* node, ProgramState*) override;
    void visit(const EndNode*, ProgramState*) override;

private:
    std::stringstream m_stream;

    // std::string get_binop(BinOpNode::Type type) const;
    // std::string get_memory_order(MemoryOrder order) const;
};

}