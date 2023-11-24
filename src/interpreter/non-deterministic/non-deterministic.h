#pragma once

#include "interpreter/interpreter.h"
#include "interpreter/state/program-state.h"

namespace wmm_simulator {

class NonDeterministicInterpreter : public Interpreter {
public:
    using Interpreter::Interpreter;

    template<class T>
    void run();
};

}