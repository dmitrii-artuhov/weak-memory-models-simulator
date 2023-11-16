#pragma once

#include <memory>


namespace wmm_simulator {

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
    virtual void visit(const AstNode*) {}
    virtual void visit(const ProgramNode*) {}
    virtual void visit(const StatementNode*) {}
    virtual void visit(const GotoNode*) {}
    virtual void visit(const ThreadGotoNode*) {}
    virtual void visit(const AssignmentNode*) {}
    virtual void visit(const NumberNode*) {}
    virtual void visit(const BinOpNode*) {}
    virtual void visit(const ConditionNode*) {}
    virtual void visit(const LoadNode*) {}
    virtual void visit(const StoreNode*) {}
    virtual void visit(const CasNode*) {}
    virtual void visit(const FaiNode*) {}
    virtual void visit(const FenceNode*) {}
    virtual void visit(const EndNode*) {}
};

}