#pragma once

#include <memory>


namespace wmm_simulator {

class ProgramState;

class AstNode;
class ProgramNode;
class StatementNode;
class AssignmentNode;
class BinOpNode;
class NumberNode;
class ConditionNode;
class CasNode;
class EndNode;
class FaiNode;
class FenceNode;
class LoadNode;
class GotoNode;
class StoreNode;
class ThreadGotoNode;

class Visitor {
public:
    virtual void visit(const AstNode*, ProgramState*) {}
    virtual void visit(const ProgramNode*, ProgramState*) {}
    virtual void visit(const StatementNode*, ProgramState*) {}
    virtual void visit(const GotoNode*, ProgramState*) {}
    virtual void visit(const ThreadGotoNode*, ProgramState*) {}
    virtual void visit(const AssignmentNode*, ProgramState*) {}
    virtual void visit(const NumberNode*, ProgramState*) {}
    virtual void visit(const BinOpNode*, ProgramState*) {}
    virtual void visit(const ConditionNode*, ProgramState*) {}
    virtual void visit(const LoadNode*, ProgramState*) {}
    virtual void visit(const StoreNode*, ProgramState*) {}
    virtual void visit(const CasNode*, ProgramState*) {}
    virtual void visit(const FaiNode*, ProgramState*) {}
    virtual void visit(const FenceNode*, ProgramState*) {}
    virtual void visit(const EndNode*, ProgramState*) {}
};

}