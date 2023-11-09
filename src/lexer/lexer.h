#pragma once

#include <vector>
#include <string>

#include "token.h"

namespace wmm_simulator {

class Lexer {
public:
    Lexer(const char* program_ptr);
    std::vector <Token> get_tokens();

private:
    const char* m_program_ptr = nullptr;
    Position m_current_position;
    

    char peek() const;
    void advance();
    Token get_next_token();
    Token get_number_token();
    Token get_identifier_token();
    Token get_atom_token(Token::Type type);

    friend std::ostream& operator<<(std::ostream&, const Token&);
};

}