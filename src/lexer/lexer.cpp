#include "lexer.h"

#include <vector>
#include <string>

#include "token.h"
#include "utils/utils.h"
#include "lang/keywords.h"

namespace wmm_simulator {

Lexer::Lexer(const char* program_ptr): m_program_ptr(program_ptr) {}

std::vector <Token> Lexer::get_tokens() {
    std::vector <Token> result;
    
    do {
        result.push_back(get_next_token());
    }
    while (!result.back().is_one_of(Token::Type::END, Token::Type::UNEXPECTED));

    return result;
}

char Lexer::peek() const {
    return *m_program_ptr;
}

void Lexer::advance() {
    if (utils::is_newline(peek())) {
        m_current_position.line++;
        m_current_position.column = 0;
    }
    else {
        m_current_position.column++;
    }

    m_current_position.index++;
    
    m_program_ptr++;
}

void Lexer::skip_blank() {
    while (utils::is_space(peek())) {
        advance();
    }
}

Token Lexer::get_next_token() {
    skip_blank();

    char c = peek();
    if (c == '\0') {
        return Token(Token::Type::END, m_current_position);
    }
    else if (utils::is_digit(c)) {
        return get_number_token();
    }
    else if (utils::is_identifier_char(c)) {
        return get_keyword_or_identifier_token();
    }
    else if (c == '=') {
        return get_atom_token(Token::Type::EQ);
    }
    else if (c == '+') {
        return get_atom_token(Token::Type::PLUS);
    }
    else if (c == '-') {
        return get_atom_token(Token::Type::MINUS);
    }
    else if (c == '*') {
        return get_atom_token(Token::Type::MULT);
    }
    else if (c == '/') {
        return get_atom_token(Token::Type::DIV);
    }
    else if (c == ':') {
        return get_atom_token(Token::Type::SEMICOLON);
    }
    else if (c == '#') {
        advance();
        return get_location_token();
    }
    else {
        return get_atom_token(Token::Type::UNEXPECTED);
    }
}

Token Lexer::get_number_token() {
    const char* lexeme_start = m_program_ptr;
    size_t len = 0;
    Position start_pos = m_current_position;

    while (utils::is_digit(peek())) {
        len++;
        advance();
    }

    return Token(Token::Type::NUMBER, lexeme_start, len, start_pos);
}

Token Lexer::get_keyword_or_identifier_token() {
    const char* lexeme_start = m_program_ptr;
    size_t len = 0;
    Position start_pos = m_current_position;

    while (utils::is_identifier_char(peek())) {
        len++;
        advance();
    }

    Token::Type type;
    std::string_view lexeme(lexeme_start, len);

    if (lexeme == Keyword::CONDITION) {
        type = Token::Type::CONDITION;
    }
    else if (lexeme == Keyword::GOTO) {
        type = Token::Type::GOTO;
    }
    else if (lexeme == Keyword::THREAD_GOTO) {
        type = Token::Type::THREAD_GOTO;
    }
    else if (
        lexeme == Keyword::SEQ_CST ||
        lexeme == Keyword::REL ||
        lexeme == Keyword::ACQ ||
        lexeme == Keyword::REL_ACQ ||
        lexeme == Keyword::RLX
    ) {
        type = Token::Type::MEMORY_ORDER;
    }
    else if (lexeme == Keyword::LOAD) {
        type = Token::Type::LOAD;
    }
    else if (lexeme == Keyword::STORE) {
        type = Token::Type::STORE;
    }
    else if (lexeme == Keyword::CAS) {
        type = Token::Type::CAS;
    }
    else if (lexeme == Keyword::FAI) {
        type = Token::Type::FAI;
    }
    else if (lexeme == Keyword::FENCE) {
        type = Token::Type::FENCE;
    }
    else {
        type = Token::Type::IDENTIFIER;
    }

    return Token(type, lexeme_start, len, start_pos);
}

Token Lexer::get_location_token() {
    if (!utils::is_identifier_char(peek())) {
        return get_atom_token(Token::Type::UNEXPECTED);
    }

    const char* lexeme_start = m_program_ptr;
    size_t len = 0;
    Position start_pos = m_current_position;

    while (utils::is_identifier_char(peek())) {
        len++;
        advance();
    }

    return Token(Token::Type::LOCATION, lexeme_start, len, start_pos);
}

Token Lexer::get_atom_token(Token::Type type) {
    const char* lexeme_start = m_program_ptr;
    Position start_pos = m_current_position;
    advance();
    
    return Token(type, lexeme_start, 1, start_pos);
}

}