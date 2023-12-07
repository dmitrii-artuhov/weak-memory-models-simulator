#pragma once

#include "interpreter/interpreter.h"
#include "utils/node-evaluator/node-evaluator.h"

namespace wmm_simulator {

class ModelCheckingInterpreter : public Interpreter {
public:
    using Interpreter::Interpreter;

    template<class T>
    std::vector<ProgramState> run();

    void visit(const StatementNode* node, ProgramState* state) override;

private:
    struct ProgramStateComparator {
        bool operator()(const int& a, const int& b) {
            return false;
        }
    };

    utils::NodeEvaluator m_evaluator;
};

}