#pragma once

#include <memory>
#include <map>
#include <algorithm>
#include <type_traits>

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

template<class T>
class Interpreter : public Visitor {
    static_assert(
        std::is_base_of<StorageSubsystem, T>::value,
        "T must be a class derived from `StorageSubsystem`."
    );

    struct ThreadState;

public:
    Interpreter(
        std::shared_ptr<ProgramNode> root,
        std::unordered_map<std::string_view, int> labeled_instructions,
        bool is_verbose = false
    );
    std::pair<
        std::shared_ptr<StorageSubsystem>,
        std::map<int, ThreadSubsystem>
    > run();
    
    void visit(const AstNode* node) override;
    void visit(const StatementNode* node) override;
    void visit(const GotoNode* node) override;
    void visit(const ThreadGotoNode* node) override;
    void visit(const AssignmentNode* node) override;
    void visit(const NumberNode* node) override;
    void visit(const BinOpNode* node) override;
    void visit(const ConditionNode* node) override;
    void visit(const LoadNode* node) override;
    void visit(const StoreNode* node) override;
    void visit(const CasNode* node) override;
    void visit(const FaiNode* node) override;
    void visit(const FenceNode* node) override;
    void visit(const EndNode* node) override;

private:
    struct ThreadState {
        int instruction_index;
        ThreadSubsystem thread_subsystem;
    };
    
    std::shared_ptr<ProgramNode> m_root;
    std::unordered_map<std::string_view, int> m_labeled_instructions;
    bool m_is_verbose;
    std::map<int, ThreadState> m_threads;
    std::map<int, ThreadSubsystem> m_finished_thread_states;
    std::shared_ptr<StorageSubsystem> m_storage;
    int m_max_thread_index = 0;
    int m_current_thread = 0;
    int m_last_evaluated_value = 0; // for visiting expressions
    int m_goto_instruction = -1; // for inner nodes to tell StatementNode that goto command was executed

    void init();
    int pick_random_thread() const;
    bool has_active_threads() const;
    void interleave_thread();
    std::string get_log_prefix();
    std::pair<
        std::shared_ptr<StorageSubsystem>,
        std::map<int, ThreadSubsystem>
    > get_state();
};

}