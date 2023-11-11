#pragma once

#include <vector>

#include "lexer/token.h"

namespace wmm_simulator {

class Parser {
public:
    Parser(std::vector<Token>&& tokens);
    

private:
    std::vector <Token> m_tokens;
    size_t m_current_token = 0;
};

}