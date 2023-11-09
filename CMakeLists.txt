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
    add_compile_options(-Wall -Wextra -Werror)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)
endif()

set(SIMULATOR_SRCS
    src/file-reader/file-reader.cpp
    src/file-reader/file-reader.h

    src/exceptions/exceptions.h

    src/lexer/lexer.cpp
    src/lexer/lexer.h
    
    src/lexer/token.cpp
    src/lexer/token.h

    src/utils/utils.h
)

set(TESTS_SRCS
    tests/initial-test.cpp
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
)
