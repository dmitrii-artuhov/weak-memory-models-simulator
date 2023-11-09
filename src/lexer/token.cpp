#include "token.h"

#include <string>

namespace wmm_simulator {

Token::Token(Type type, Position pos): m_type(type), m_start_pos(pos), m_len(0), m_lexeme("") {}

Token::Token(Type type, const char* lexeme_start, size_t len, Position pos): m_type(type), m_start_pos(pos), m_len(len), m_lexeme(lexeme_start, len) {}

const std::string_view Token::get_lexeme() const {
    return m_lexeme;
}

bool Token::is(Type type) const {
    return m_type == type;
}

bool Token::is_one_of(Type t1, Type t2) const {
    return is(t1) || is(t2);
}

std::string Token::get_readable_type() const {
    switch (m_type) {
        case Type::IDENTIFIER: return "IDENTIFIER";
        case Type::NUMBER: return "NUMBER";
        case Type::EQ: return "EQ";
        case Type::PLUS: return "PLUS";
        case Type::MINUS: return "MINUS";
        case Type::MULT: return "MULT";
        case Type::DIV: return "DIV";
        case Type::SEMICOLON: return "SEMICOLON";
        case Type::HASHTAG: return "HASHTAG";
        case Type::END: return "END";
        case Type::UNEXPECTED: return "UNEXPECTED";
        default: return "unreachable";
    }
}

Position Token::get_start_position() const {
    return m_start_pos;
}

}
