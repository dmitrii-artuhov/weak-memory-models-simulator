#pragma once

#include <stdexcept>
#include <vector>

#include "lexer/token.h"

namespace wmm_simulator::exceptions {
    struct invalid_arguments : std::runtime_error {
        explicit invalid_arguments(const std::string& msg) : std::runtime_error(msg) {}
    };

    struct unable_to_open_file : std::runtime_error {
        explicit unable_to_open_file(const std::string& filename) : std::runtime_error("Unable to open file at '" + filename + "'") {}
    };

    struct invalid_token : std::runtime_error {
        explicit invalid_token(const Token& token): std::runtime_error("Invalid token '" + std::string(token.get_lexeme()) + "' at: " + token.get_start_position().as_string()) {}
    };

    struct unexpected_token : std::runtime_error {
        explicit unexpected_token(const Token& token, std::vector<Token::Type> expected_types): std::runtime_error("Unexpected token '" + std::string(token.get_lexeme()) + "', with type " + token.get_readable_type() + " at: " + token.get_start_position().as_string() + ". Expected type(s): " + unexpected_token::get_unexpected_token_msg(expected_types)) {}

    private:
        static std::string get_unexpected_token_msg(std::vector<Token::Type> expected_types) {
            std::string expected_types_msg = "";

            for (size_t i = 0; i < expected_types.size(); ++i) {
                auto type = expected_types[i];
                expected_types_msg += Token::to_readable_type(type);

                if (i != expected_types.size() - 1) {
                    expected_types_msg += ", ";
                }
            }

            return expected_types_msg;
        }
    };

    struct multiple_statements_with_same_label : std::runtime_error {
        explicit multiple_statements_with_same_label(const std::string& label_name) : std::runtime_error("Multiple statements have the same label: '" + label_name + "'") {}
    };

    struct unknown_label : std::runtime_error {
        explicit unknown_label(const std::string& label_name) : std::runtime_error("Unknown label: '" + label_name + "'") {}
    };
    
}
