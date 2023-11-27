#pragma once

#include <random>
#include <string>

#include "lang/keywords.h"
#include "lang/memory-order.h"
#include "ast/binop/binop.h"

namespace wmm_simulator::utils {

bool is_space(char c);
bool is_newline(char c);
bool is_digit(char c);
bool is_identifier_char(char c);
int get_random_in_range(int left, int right);
std::string trim(std::string_view str);
std::string get_binop(BinOpNode::Type type);
std::string get_memory_order(MemoryOrder order);
}