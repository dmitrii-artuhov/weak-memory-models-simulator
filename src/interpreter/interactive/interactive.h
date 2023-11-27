#pragma once

#include "interpreter/interpreter.h"
#include "interpreter/tracing/tracing.h"

namespace wmm_simulator {

class InteractiveInterpreter : TracingInterpreter {
public:
    using TracingInterpreter::TracingInterpreter;

    template<class T>
    void run();
};

}