#include <iostream>
#include <vector>

#include "exceptions/exceptions.h"
#include "file-reader/file-reader.h"
#include "lexer/token.h"
#include "lexer/lexer.h"

int main(int argc, char* argv[]) {
    using namespace wmm_simulator;
    
    try {
        if (argc != 2) {
            throw exceptions::invalid_arguments("USAGE: " + std::string(argv[0]) + " [path/to/program]");
        }
    
        std::string program_path = argv[1];
        std::string program_text = fs::FileReader::read_file(program_path);

        std::cout << "Program text: \n" << program_text << std::endl;

        std::string code =
        "r = 1\n"
        "r1 = r2 + 100\n"
        "store   \t SQ #r1 r2\n"
        "L1: fence SQ\n"
        "";

        Lexer lexer(code.c_str());
        std::vector <Token> tokens = lexer.get_tokens();

        for (auto& token : tokens) {
            std::cout << token << std::endl;
        }
    }
    catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}