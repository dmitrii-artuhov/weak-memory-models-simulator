#include "doctest.h"

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "exceptions/exceptions.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/node.h"
#include "visitor/visitor.h"
#include "utils/node-stringifier/node-stringifier.h"

namespace doctest {
using namespace wmm_simulator;

static std::string get_code(const ProgramNode* node) {
    utils::NodeStringifier stringifier;
    std::string code = "";

    for (const auto& statement : node->get_statements()) {
        code += stringifier.stringify(statement.get()) + "\n";
    }

    return code;
}

static void check_code_same(const std::string& got, const std::string& expected) {
    CHECK(expected + "end\n" == got);
}

template <class ExceptionType>
static void check_throws_for_code(const std::string& code) {
    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());
    CHECK_THROWS_AS(parser.parse(), ExceptionType);
}

TEST_CASE("Parser: assignment") {
    std::string code =
        "r = 1\n"
        "r1 = r2 + r3\n"
        "goto L1\n"
        "L1: store SEQ_CST #x r\n"
        "r1 = cas SEQ_CST #x r2 r3\n"
        "r1 = fai SEQ_CST #x r2\n";

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());

    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > ast = parser.parse();

    CHECK(ast.second.size() == 1);
    CHECK(ast.second.size() == 1);
    CHECK(ast.second.count("L1"));
    CHECK(ast.second["L1"] == 3);

    check_code_same(get_code(ast.first.get()), code);
}

TEST_CASE("Parser: fence") {
    std::string code =
        "fence SEQ_CST\n"
        "fence REL\n"
        "fence ACQ\n"
        "fence REL_ACQ\n";
        "fence RLX\n";

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());

    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > ast = parser.parse();

    CHECK(ast.second.empty());

    check_code_same(get_code(ast.first.get()), code);
}


TEST_CASE("Parser: invalid programs") {
    check_throws_for_code<exceptions::unexpected_token>("r = ");
    check_throws_for_code<exceptions::unexpected_token>("r = r1 + ");
    check_throws_for_code<exceptions::unexpected_token>("if r goto if");
    check_throws_for_code<exceptions::unexpected_token>("load m x r");
    check_throws_for_code<exceptions::unexpected_token>("load SEQ_CST x r");
    check_throws_for_code<exceptions::unexpected_token>("store REL x r");
    check_throws_for_code<exceptions::unexpected_token>("fence #x");
    check_throws_for_code<exceptions::unexpected_token>("thread_goto RLX");
    check_throws_for_code<exceptions::unexpected_token>("L1 r = 1");

    check_throws_for_code<exceptions::invalid_token>("r = r1 ~ r2");
    check_throws_for_code<exceptions::invalid_token>("r1 = cas ACQ #x r2 ?");
    check_throws_for_code<exceptions::invalid_token>("r1 = fai m #x r2 ?");
}

}