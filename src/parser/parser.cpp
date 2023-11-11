#include "parser.h"

#include <vector>

namespace wmm_simulator {

Parser::Parser(std::vector <Token>&& tokens): m_tokens(std::move(tokens)) {}



}