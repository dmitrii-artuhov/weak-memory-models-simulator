#pragma once

#include <random>

namespace wmm_simulator::utils {

bool is_space(char c);
bool is_newline(char c);
bool is_digit(char c);
bool is_identifier_char(char c);
int get_random_in_range(int left, int right);

}