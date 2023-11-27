#include "utils.h"

#include <cctype>
#include <string>

#include "lang/keywords.h"
#include "lang/memory-order.h"
#include "ast/binop/binop.h"

namespace wmm_simulator::utils {

bool is_space(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

bool is_newline(char c) {
    return c == '\n';
}

bool is_digit(char c) {
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

bool is_identifier_char(char c) {
    switch (c) {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '_':
            return true;
        default:
            return false;
    }
}

int get_random_in_range(int left, int right) {
    static std::random_device rd;
    static std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dist{left, right};
    
    return dist(gen);
}

// removes whites spaces from start and end of `str`, but preservers newline character
std::string trim(std::string_view str) {
    size_t start = 0;
    size_t end = str.length();

    // Find the first non-whitespace character from the beginning
    while (start < end && str[start] != '\n' && std::isspace(str[start])) {
        start++;
    }

    // Find the first non-whitespace character from the end
    while (end > start && str[end - 1] != '\n' && std::isspace(str[end - 1])) {
        end--;
    }

    // Extract the substring without leading and trailing whitespaces
    return std::string(str.substr(start, end - start));
}

std::string get_binop(BinOpNode::Type type) {
    switch (type) {
        case BinOpNode::Type::PLUS: return "+";
        case BinOpNode::Type::MINUS: return "-";
        case BinOpNode::Type::MULT: return "*";
        case BinOpNode::Type::DIV: return "/";
    }

    return "?";
}

std::string get_memory_order(MemoryOrder order) {
    switch (order) {
        case MemoryOrder::SEQUENTIALLY_CONSISTENT: return Keyword::SEQ_CST;
        case MemoryOrder::ACQUIRE: return Keyword::ACQ;
        case MemoryOrder::RELEASE: return Keyword::REL;
        case MemoryOrder::RELEASE_ACQUIRE: return Keyword::REL_ACQ;
        case MemoryOrder::RELAXED: return Keyword::RLX;
    }

    return "?";
}

}