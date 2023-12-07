#pragma once

#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <type_traits>

// #include "program-state.h"
#include "thread-subsystem/thread-subsystem.h"
#include "storage-subsystem/storage-subsystem.h"
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

namespace wmm_simulator {
class ProgramState;

class Interpreter : public Visitor {
public:
    template<class T>
    std::vector<ProgramState> run();

    Interpreter(
        std::shared_ptr<ProgramNode> root,
        std::unordered_map<std::string_view, int> labeled_instructions
    );

    const ProgramNode* get_ast() const;
    
    void visit(const AstNode* node, ProgramState* state) override;
    void visit(const StatementNode* node, ProgramState* state) override;
    void visit(const GotoNode* node, ProgramState* state) override;
    void visit(const ThreadGotoNode* node, ProgramState* state) override;
    void visit(const AssignmentNode* node, ProgramState* state) override;
    void visit(const NumberNode* node, ProgramState* state) override;
    void visit(const BinOpNode* node, ProgramState* state) override;
    void visit(const ConditionNode* node, ProgramState* state) override;
    void visit(const LoadNode* node, ProgramState* state) override;
    void visit(const StoreNode* node, ProgramState* state) override;
    void visit(const CasNode* node, ProgramState* state) override;
    void visit(const FaiNode* node, ProgramState* state) override;
    void visit(const FenceNode* node, ProgramState* state) override;
    void visit(const EndNode* node, ProgramState* state) override;

    std::shared_ptr<ProgramNode> m_root;
    std::unordered_map<std::string_view, int> m_labeled_instructions;

    int get_random_active_thread_id(const ProgramState& state) const;
    void print_state(const ProgramState& state) const;
    // void init();
    // int pick_random_thread() const;
    // bool has_active_threads() const;
    // void interleave_thread();
    // std::string get_log_prefix();
    // std::pair<
    //     std::shared_ptr<StorageSubsystem>,
    //     std::map<int, ThreadSubsystem>
    // > get_state();
};

}