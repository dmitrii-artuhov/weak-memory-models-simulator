#include "doctest.h"

#include <iostream>
#include <string>
#include "lexer/lexer.h"
#include "lexer/token.h"

namespace doctest {
using namespace wmm_simulator;

static void check_tokens_same(const Token& t1, const Token& t2) {
    CHECK(t1.get_length() == t2.get_length());
    CHECK(t1.get_type() == t2.get_type());
    CHECK(t1.get_start_position() == t2.get_start_position());
    CHECK(t1.get_lexeme() == t2.get_lexeme());
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

    Lexer lexer(code.c_str());
    std::vector <Token> tokens = lexer.get_tokens();

    CHECK(tokens.size() == expected.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        check_tokens_same(tokens[i], expected[i]);
    }
}

TEST_CASE("Lexer: test empty code") {
    using namespace wmm_simulator;

    std::string code = "";

    std::vector <Token> expected = {
        Token(Token::Type::END, Position{0, 0, 0})
    };

    Lexer lexer(code.c_str());
    std::vector <Token> tokens = lexer.get_tokens();

    CHECK(tokens.size() == expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        check_tokens_same(tokens[i], expected[i]);
    }
}

TEST_CASE("Lexer: test shifted empty code") {
    using namespace wmm_simulator;

    std::string code = "\n\t\n";

    std::vector <Token> expected = {
        Token(Token::Type::END, Position{3, 0, 2})
    };

    Lexer lexer(code.c_str());
    std::vector <Token> tokens = lexer.get_tokens();

    CHECK(tokens.size() == expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        check_tokens_same(tokens[i], expected[i]);
    }
}

TEST_CASE("Lexer: test code with unexpected token") {
    using namespace wmm_simulator;

    std::string code =
        "r = 1\n"
        "store SEQ_CST #x r~!\n";

    std::vector <Token> expected = {
        Token(Token::Type::IDENTIFIER, code.c_str(), 1, Position{0, 0, 0}),
        Token(Token::Type::EQ, code.c_str() + 2, 1, Position{2, 2, 0}),
        Token(Token::Type::NUMBER, code.c_str() + 4, 1, Position{4, 4, 0}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 6, 5, Position{6, 0, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 12, 7, Position{12, 6, 1}),
        Token(Token::Type::HASHTAG, code.c_str() + 20, 1, Position{20, 14, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 21, 1, Position{21, 15, 1}),
        Token(Token::Type::IDENTIFIER, code.c_str() + 23, 1, Position{23, 17, 1}),
        Token(Token::Type::UNEXPECTED, code.c_str() + 24, 1, Position{24, 18, 1})
    };

    Lexer lexer(code.c_str());
    std::vector <Token> tokens = lexer.get_tokens();

    CHECK(tokens.size() == expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        check_tokens_same(tokens[i], expected[i]);
    }
}
}