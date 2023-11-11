#pragma once

#include <iostream>
#include <string>

namespace wmm_simulator {

struct Position {
    size_t index; // global start of the token
    size_t column; // start of the token in line
    size_t line; // number of line

    std::string as_string() const {
        return "(index: " + std::to_string(index) + ", column: " + std::to_string(column) + ", line: " + std::to_string(line) + ")";
    } 
};
    
class Token {
public:
    enum class Type {
        IDENTIFIER, // register or label
        LOCATION,
        NUMBER,
        
        EQ,
        PLUS,
        MINUS,
        MULT,
        DIV,
        SEMICOLON,

        CONDITION,
        GOTO,
        THREAD_GOTO,
        MEMORY_ORDER,

        LOAD,
        STORE,
        CAS,
        FAI,
        FENCE,
        
        END,
        INVALID
    };

    Token(Type type, Position pos);
    Token(Type type, const char* lexeme_start, size_t len, Position pos);
    const std::string_view get_lexeme() const;
    bool is(Type type) const;
    bool is_one_of(Type t1, Type t2) const;
    std::string get_readable_type() const;
    static std::string to_readable_type(Type type);
    Position get_start_position() const;
    size_t get_length() const;
    Type get_type() const;

    template <typename... Ts>
    bool is_one_of(Type t1, Type t2, Ts... ts) const {
        return is(t1) || is_one_of(t2, ts...);
    }

private:
    Type m_type;
    Position m_start_pos;
    size_t m_len;
    std::string_view m_lexeme;
};

inline std::ostream& operator<<(std::ostream& os, const Position& pos) {
    os << "Position" << pos.as_string();
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << token.get_readable_type() << ", '" << token.get_lexeme() << "', len: " << token.get_length() << ", " << token.get_start_position() <<  ")";
    return os;
}

inline bool operator==(const Position& p1, const Position& p2) {
    return (
        p1.index == p2.index &&
        p1.column == p2.column &&
        p1.line == p2.line
    );
}
}