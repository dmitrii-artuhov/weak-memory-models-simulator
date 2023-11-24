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
#include "interpreter/non-deterministic/non-deterministic.h"
#include "interpreter/tracing/tracing.h"
#include "interpreter/model-checking/model-checking.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"

int main([[ maybe_unused ]] int argc, [[ maybe_unused ]] char* argv[]) {
    using namespace wmm_simulator;
    
    try {
        // if (argc != 2) {
        //     throw exceptions::invalid_arguments("USAGE: " + std::string(argv[0]) + " [path/to/program]");
        // }
    
        // std::string program_path = argv[1];
        // std::string program_path = "../tests/test-data/single-thread.txt";
        // std::string program_path = "../tests/test-data/message-passing.txt";
        std::string program_path = "../tests/test-data/store-buffering.txt";
        std::string code = fs::FileReader::read_file(program_path);

        std::cout << "Program text: \n" << code << std::endl;
        
        Lexer lexer(code.c_str());
        std::vector <Token> tokens = lexer.get_tokens();

        Parser parser(tokens);
        std::pair<
            std::shared_ptr<ProgramNode>,
            std::unordered_map<std::string_view, int>
        > parse_result = parser.parse();

        // NonDeterministicInterpreter interpreter(parse_result.first, parse_result.second);
        // TracingInterpreter interpreter(parse_result.first, parse_result.second);
        ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
        
        interpreter.run<SCStorageSubsystem>();
        // interpreter.run<TSOStorageSubsystem>();
        // interpreter.run<PSOStorageSubsystem>();
    }
    catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}