#include "doctest.h"

#include <iostream>
#include <string>
#include "lexer/lexer.h"
#include "lexer/token.h"

namespace doctest {
using namespace wmm_simulator;

static void check_tokens_same(const Token& got, const Token& expected) {
    CHECK(got.get_length() == expected.get_length());
    CHECK(got.get_type() == expected.get_type());
    CHECK(got.get_start_position() == expected.get_start_position());
    CHECK(got.get_lexeme() == expected.get_lexeme());
}

static void check_lexer_result(const std::string& code, const std::vector <Token>& expected) {
    Lexer lexer(code.c_str());
    std::vector <Token> tokens = lexer.get_tokens();

    CHECK(tokens.size() == expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        check_tokens_same(tokens[i], expected[i]);
    }
}



TEST_CASE("Lexer: test assignments") {
    std::string code =
        "r = 100\n"
        "r1 = r+1\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 1, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::NUMBER, code.c_str() + 4, 3, Position{4, 4, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 8, 2, Position{8, 0, 1}),
        Token(Token::Type::EQ, code.c_str() + 11, 1, Position{11, 3, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 13, 1, Position{13, 5, 1}),
        Token(Token::Type::PLUS, code.c_str() + 14, 1, Position{14, 6, 1}),
        Token(Token::Type::NUMBER, code.c_str() + 15, 1, Position{15, 7, 1}),
        Token(Token::Type::END, Position{17, 0, 2})
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test empty code") {
    

    std::string code = "";

    std::vector <Token> expected = {
        Token(Token::Type::END, Position{0, 0, 0})
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test shifted empty code") {
    

    std::string code = "\n\t\n";

    std::vector <Token> expected = {
        Token(Token::Type::END, Position{3, 0, 2})
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test location") {
    

    std::string code = "r #loc";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 1, Position{0, 0, 0}),
        Token(Token::Type::LOCATION, code.c_str() + 3, 3, Position{3, 3, 0}),
        Token(Token::Type::END, Position{6, 6, 0}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test plus operation") {
    std::string code =
        "r1=r2+r3\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 2, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 3, 2, Position{3, 3, 0}),
        Token(Token::Type::PLUS, code.c_str() + 5, 1, Position{5, 5, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 6, 2, Position{6, 6, 0}),
        Token(Token::Type::END, Position{9, 0, 1}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test minus operation") {
    

    std::string code =
        "r1=r2-r3\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 2, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 3, 2, Position{3, 3, 0}),
        Token(Token::Type::MINUS, code.c_str() + 5, 1, Position{5, 5, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 6, 2, Position{6, 6, 0}),
        Token(Token::Type::END, Position{9, 0, 1}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test mult operation") {
    

    std::string code =
        "r1=r2*r3\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 2, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 3, 2, Position{3, 3, 0}),
        Token(Token::Type::MULT, code.c_str() + 5, 1, Position{5, 5, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 6, 2, Position{6, 6, 0}),
        Token(Token::Type::END, Position{9, 0, 1}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test div operation") {
    

    std::string code =
        "r1=r2/r3\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 2, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 3, 2, Position{3, 3, 0}),
        Token(Token::Type::DIV, code.c_str() + 5, 1, Position{5, 5, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 6, 2, Position{6, 6, 0}),
        Token(Token::Type::END, Position{9, 0, 1}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test condition") {
    

    std::string code = "if r goto L";

    std::vector <Token> expected = {
        Token(Token::Type::CONDITION, code.c_str(), 2, Position{0, 0, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 3, 1, Position{3, 3, 0}),
        Token(Token::Type::GOTO, code.c_str() + 5, 4, Position{5, 5, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 10, 1, Position{10, 10, 0}),
        Token(Token::Type::END, Position{11, 11, 0}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test goto & thread_goto") {
    

    std::string code =
        "goto L1\n"
        "L1: thread_goto T1\n"
        "T1: r = 1\n";

    std::vector <Token> expected = {
        Token(Token::Type::GOTO, code.c_str(), 4, Position{0, 0, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 5, 2, Position{5, 5, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 8, 2, Position{8, 0, 1}),
        Token(Token::Type::SEMICOLON, code.c_str() + 10, 1, Position{10, 2, 1}),
        Token(Token::Type::THREAD_GOTO, code.c_str() + 12, 11, Position{12, 4, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 24, 2, Position{24, 16, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 27, 2, Position{27, 0, 2}),
        Token(Token::Type::SEMICOLON, code.c_str() + 29, 1, Position{29, 2, 2}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 31, 1, Position{31, 4, 2}),
        Token(Token::Type::EQ, code.c_str() + 33, 1, Position{33, 6, 2}),
        Token(Token::Type::NUMBER, code.c_str() + 35, 1, Position{35, 8, 2}),
        Token(Token::Type::END, Position{37, 0, 3}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test memory orders") {
    std::string code = "SEQ_CST REL ACQ REL_ACQ RLX";

    std::vector <Token> expected = {
        Token(Token::Type::MEMORY_ORDER, code.c_str(), 7, Position{0, 0, 0}),
        Token(Token::Type::MEMORY_ORDER, code.c_str() + 8, 3, Position{8, 8, 0}),
        Token(Token::Type::MEMORY_ORDER, code.c_str() + 12, 3, Position{12, 12, 0}),
        Token(Token::Type::MEMORY_ORDER, code.c_str() + 16, 7, Position{16, 16, 0}),
        Token(Token::Type::MEMORY_ORDER, code.c_str() + 24, 3, Position{24, 24, 0}),
        Token(Token::Type::END, Position{27, 27, 0})
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test keywords") {
    std::string code = "load store cas fai fence";

    std::vector <Token> expected = {
        Token(Token::Type::LOAD, code.c_str(), 4, Position{0, 0, 0}),
        Token(Token::Type::STORE, code.c_str() + 5, 5, Position{5, 5, 0}),
        Token(Token::Type::CAS, code.c_str() + 11, 3, Position{11, 11, 0}),
        Token(Token::Type::FAI, code.c_str() + 15, 3, Position{15, 15, 0}),
        Token(Token::Type::FENCE, code.c_str() + 19, 5, Position{19, 19, 0}),
        Token(Token::Type::END, Position{24, 24, 0}),
    };

    check_lexer_result(code, expected);
}

TEST_CASE("Lexer: test code with unexpected token") {
    

    std::string code =
        "r = 1\n"
        "store SEQ_CST #x r~!\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 1, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::NUMBER, code.c_str() + 4, 1, Position{4, 4, 0}),
        Token(Token::Type::STORE, code.c_str() + 6, 5, Position{6, 0, 1}),
        Token(Token::Type::MEMORY_ORDER, code.c_str() + 12, 7, Position{12, 6, 1}),
        Token(Token::Type::LOCATION, code.c_str() + 21, 1, Position{21, 15, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 23, 1, Position{23, 17, 1}),
        Token(Token::Type::UNEXPECTED, code.c_str() + 24, 1, Position{24, 18, 1})
    };

    check_lexer_result(code, expected);
}

}