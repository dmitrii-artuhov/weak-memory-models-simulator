#pragma once

#include "interpreter/interpreter.h"
#include "state/program-state.h"

namespace wmm_simulator {

class NonDeterministicInterpreter : public Interpreter {
public:
    using Interpreter::Interpreter;

    template<class T>
    std::vector<ProgramState> run();
};

}