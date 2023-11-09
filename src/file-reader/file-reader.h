#pragma once

#include <string>

#include "exceptions/exceptions.h"

namespace wmm_simulator::fs {

class FileReader {
public:
    static std::string read_file(const std::string& path);
};

}