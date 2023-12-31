#pragma once

#include <string>

#include "interpreter/interpreter.h"
#include "state/program-state.h"
#include "ast/program/program.h"
#include "utils/node-stringifier/node-stringifier.h"

namespace wmm_simulator {

class TracingInterpreter : public Interpreter {
public:
    using Interpreter::Interpreter;

    template<class T>
    std::vector<ProgramState> run();

    void visit(const StatementNode* node, ProgramState* state) override;

protected:
    std::string get_log_prefix(int thread_id) const;
    utils::NodeStringifier m_stringifier;
};

}