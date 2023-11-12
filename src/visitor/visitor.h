#pragma once

#include <memory>


namespace wmm_simulator {

class AstNode;
class ProgramNode;
class StatementNode;

class Visitor {
public:
    virtual void visit(const AstNode* node) const = 0;
    virtual void visit(const ProgramNode* node) const = 0;
    virtual void visit(const StatementNode* node) const = 0;
};

}