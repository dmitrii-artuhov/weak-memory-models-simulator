#pragma once

#include <stdexcept>

namespace wmm_simulator::exceptions {
    struct invalid_arguments : std::runtime_error {
        explicit invalid_arguments(const std::string& msg) : std::runtime_error(msg) {}
    };

    struct unable_to_open_file : std::runtime_error {
        explicit unable_to_open_file(const std::string& filename) : std::runtime_error("Unable to open file at '" + filename + "'") {}
    };
}
