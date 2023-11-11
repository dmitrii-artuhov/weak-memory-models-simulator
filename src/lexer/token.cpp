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

Token::Type Token::get_type() const {
    return m_type;
}

std::string Token::get_readable_type() const {
    return Token::to_readable_type(m_type);
}

std::string Token::to_readable_type(Type type) {
    switch (type) {
        case Type::IDENTIFIER: return "IDENTIFIER";
        case Type::LOCATION: return "LOCATION";
        
        case Type::NUMBER: return "NUMBER";

        case Type::EQ: return "EQ";
        case Type::PLUS: return "PLUS";
        case Type::MINUS: return "MINUS";
        case Type::MULT: return "MULT";
        case Type::DIV: return "DIV";
        case Type::SEMICOLON: return "SEMICOLON";
        
        case Type::CONDITION: return "CONDITION";
        case Type::GOTO: return "GOTO";
        case Type::THREAD_GOTO: return "THREAD_GOTO";
        case Type::MEMORY_ORDER: return "MEMORY_ORDER";

        case Type::LOAD: return "LOAD";
        case Type::STORE: return "STORE";
        case Type::CAS: return "CAS";
        case Type::FAI: return "FAI";
        case Type::FENCE: return "FENCE";

        case Type::END: return "END";
        case Type::INVALID: return "INVALID";
        default: return "unreachable";
    }
}

Position Token::get_start_position() const {
    return m_start_pos;
}

size_t Token::get_length() const {
    return m_len;
}

}
