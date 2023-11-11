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
    Position m_current_position = {0, 0, 0};
    

    char peek() const;
    void advance();
    void skip_blank();
    Token get_next_token();
    Token get_number_token();
    Token get_keyword_or_identifier_token();
    Token get_location_token();
    Token get_atom_token(Token::Type type);

    friend std::ostream& operator<<(std::ostream&, const Token&);
};

}