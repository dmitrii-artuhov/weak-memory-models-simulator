#pragma once

#include <memory>
#include <unordered_map>
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
#include "ast/goto/goto.h"
#include "ast/thread-goto/thread-goto.h"
#include "ast/end/end.h"

namespace wmm_simulator {

// template<class StorageSubsystemType>
// using IsBaseOfStorageSubsystemType = std::enable_if_t<std::is_base_of<StorageSubsystem, StorageSubsystemType>::value>;

template<class T>
class Interpreter : public Visitor {
    static_assert(
        std::is_base_of<StorageSubsystem, T>::value,
        "T must be classes derived from StorageSubsystem."
    );

    struct ThreadState;

public:
    Interpreter(
        std::shared_ptr<ProgramNode> root,
        std::unordered_map<std::string_view, int> labeled_instructions
    );
    std::shared_ptr<StorageSubsystem> run();
    
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
    void visit(const EndNode* node) override;

private:
    struct ThreadState {
        int instruction_index;
        ThreadSubsystem thread_subsystem;
    };
    
    std::shared_ptr<ProgramNode> m_root;
    std::unordered_map<std::string_view, int> m_labeled_instructions;
    std::unordered_map <int, ThreadState> m_threads;
    std::shared_ptr<StorageSubsystem> m_storage;
    int m_max_thread_index = 0;
    int m_current_thread = 0;
    int m_last_evaluated_value; // for visiting expressions

    bool has_active_threads() const;
};

}