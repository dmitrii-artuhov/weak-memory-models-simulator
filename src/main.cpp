#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "exceptions/exceptions.h"
#include "file-reader/file-reader.h"
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"

int main([[ maybe_unused ]] int argc, [[ maybe_unused ]] char* argv[]) {
    using namespace wmm_simulator;
    
    try {
        // if (argc != 2) {
        //     throw exceptions::invalid_arguments("USAGE: " + std::string(argv[0]) + " [path/to/program]");
        // }
    
        // std::string program_path = argv[1];
        // std::string program_text = fs::FileReader::read_file(program_path);

        // std::cout << "Program text: \n" << program_text << std::endl;

        std::string code =
        "L1: r = 1\n";

        Lexer lexer(code.c_str());
        std::vector <Token> tokens = lexer.get_tokens();

        for (auto& token : tokens) {
            std::cout << token << std::endl;
        }

        std::cout << std::endl << "Parsing:" << std::endl;

        Parser parser(tokens);
        std::pair<
            std::shared_ptr<ProgramNode>,
            std::unordered_map<std::string_view, int>
        > parse_result = parser.parse();

        Interpreter<SCStorageSubsystem> interpreter(parse_result.first, parse_result.second);
        auto storage = interpreter.run();
    }
    catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}