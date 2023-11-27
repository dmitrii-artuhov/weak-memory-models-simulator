cmake_minimum_required(VERSION 3.15)

project(WeakMemoryModelsSimulator CXX)

#### setting global props
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#### Setting targets
set(SIMULATOR_TARGET simulator)
set(TESTS_TARGET tests)

#### compile options for all targets
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options() #-Wall -Wextra -Werror # why these cause the build crash -fsanitize=address -fsanitize=leak ?
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)
endif()

set(SIMULATOR_SRCS
    src/file-reader/file-reader.cpp
    src/file-reader/file-reader.h

    src/args-parser/args-parser.h

    src/exceptions/exceptions.h
    
    src/utils/utils.cpp
    src/utils/utils.h

    src/utils/node-stringifier/node-stringifier.cpp
    src/utils/node-stringifier/node-stringifier.h
    
    src/utils/node-evaluator/node-evaluator.cpp
    src/utils/node-evaluator/node-evaluator.h

    src/parser/parser.cpp
    src/parser/parser.h

    src/lexer/lexer.cpp
    src/lexer/lexer.h
    
    src/lexer/token.cpp
    src/lexer/token.h

    src/lang/keywords.h
    src/lang/memory-order.h
    
    src/visitor/visitor.h

    src/interpreter/interpreter.cpp
    src/interpreter/interpreter.h

    src/interpreter/non-deterministic/non-deterministic.cpp
    src/interpreter/non-deterministic/non-deterministic.h
    
    src/interpreter/tracing/tracing.cpp
    src/interpreter/tracing/tracing.h
    
    src/interpreter/interactive/interactive.cpp
    src/interpreter/interactive/interactive.h
    src/interpreter/interactive/interactive-cli.cpp
    src/interpreter/interactive/interactive-cli.h
    
    src/interpreter/model-checking/model-checking.cpp
    src/interpreter/model-checking/model-checking.h

    src/state/program-state.cpp
    src/state/program-state.h

    src/thread-subsystem/thread-subsystem.cpp
    src/thread-subsystem/thread-subsystem.h
    
    # storage subsystems
    src/storage-subsystem/storage-subsystem.h

    src/storage-subsystem/sc/sc-storage-subsystem.cpp
    src/storage-subsystem/sc/sc-storage-subsystem.h
    
    src/storage-subsystem/tso/tso-storage-subsystem.cpp
    src/storage-subsystem/tso/tso-storage-subsystem.h
    
    src/storage-subsystem/pso/pso-storage-subsystem.cpp
    src/storage-subsystem/pso/pso-storage-subsystem.h
    
    src/storage-subsystem/sra/sra-storage-subsystem.cpp
    src/storage-subsystem/sra/sra-storage-subsystem.h

    # ast nodes
    src/ast/node.h

    src/ast/program/program.cpp
    src/ast/program/program.h
    
    src/ast/statement/statement.cpp
    src/ast/statement/statement.h

    src/ast/assignment/assignment.cpp
    src/ast/assignment/assignment.h

    src/ast/binop/binop.cpp
    src/ast/binop/binop.h

    src/ast/number/number.cpp
    src/ast/number/number.h

    src/ast/condition/condition.cpp
    src/ast/condition/condition.h
    
    src/ast/goto/goto.cpp
    src/ast/goto/goto.h

    src/ast/thread-goto/thread-goto.cpp
    src/ast/thread-goto/thread-goto.h
    
    src/ast/load/load.cpp
    src/ast/load/load.h
    
    src/ast/store/store.cpp
    src/ast/store/store.h
    
    src/ast/cas/cas.cpp
    src/ast/cas/cas.h
    
    src/ast/fai/fai.cpp
    src/ast/fai/fai.h

    src/ast/fence/fence.cpp
    src/ast/fence/fence.h
    
    src/ast/end/end.cpp
    src/ast/end/end.h
)

set(TESTS_SRCS
    tests/test-lexer.cpp
    tests/test-parser.cpp
    ${SIMULATOR_SRCS}
)

#### configuring SIMULATOR_TARGET
add_executable(${SIMULATOR_TARGET}
    src/main.cpp
    ${SIMULATOR_SRCS}
)

target_include_directories(${SIMULATOR_TARGET} PUBLIC
    src
)

#### configuring TESTS_TARGET:
add_executable(${TESTS_TARGET}
    src/doctest-main.cpp
    ${TESTS_SRCS}
)

target_include_directories(${TESTS_TARGET} PUBLIC
    ./doctest
    src
)
