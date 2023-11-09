#include "file-reader.h"

#include <string>
#include <fstream>

namespace wmm_simulator::fs {
std::string FileReader::read_file(const std::string& path) {
    std::fstream file;
    file.open(path, std::ios::in);

    if (!file || !file.is_open()) {
        throw wmm_simulator::exceptions::unable_to_open_file(path);
    }

    std::string text = "";
    std::string line = "";

    while (!file.eof()) {
        std::getline(file, line);
        text += line + "\n";
    }

    file.close();
    return text;
}
}