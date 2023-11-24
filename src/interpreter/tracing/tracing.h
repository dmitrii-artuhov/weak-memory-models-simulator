#pragma once

#include <string>

#include "interpreter/interpreter.h"
#include "interpreter/state/program-state.h"
#include "ast/program/program.h"
#include "utils/node-stringifier/node-stringifier.h"

namespace wmm_simulator {

class TracingInterpreter : public Interpreter {
public:
    using Interpreter::Interpreter;

    template<class T>
    void run();

    void visit(const StatementNode* node, ProgramState* state) override;

private:
    std::string get_log_prefix(int thread_id) const;
    utils::NodeStringifier m_stringifier;
};

}