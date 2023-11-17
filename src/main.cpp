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
#include "storage-subsystem/tso/tso-storage-subsystem.h"

int main([[ maybe_unused ]] int argc, [[ maybe_unused ]] char* argv[]) {
    using namespace wmm_simulator;
    
    try {
        // if (argc != 2) {
        //     throw exceptions::invalid_arguments("USAGE: " + std::string(argv[0]) + " [path/to/program]");
        // }
    
        // std::string program_path = argv[1];
        // std::string program_path = "../tests/test-data/message-passing.txt";
        std::string program_path = "../tests/test-data/store-buffering.txt";
        std::string code = fs::FileReader::read_file(program_path);


        // std::string code =
        // "a = 3\n"
        // "store SEQ_CST #x a\n"
        // "a = 5\n"
        // "c = 7\n"
        // "b = cas SEQ_CST #x a c\n";
        // "b = fai SEQ_CST #x a\n";

        std::cout << "Program text: \n" << code << std::endl;
        
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

        std::cout << "Labels table:" << std::endl;
        for (auto [ label, instruction_index ] : parse_result.second) {
            std::cout << label << ": " << instruction_index << std::endl;
        }
        std::cout << std::endl;

        Interpreter<TSOStorageSubsystem> interpreter(parse_result.first, parse_result.second);
        auto state = interpreter.run();

        std::cout << std::endl << "=========== Memory state ===========" << std::endl;
        for (auto& [ loc, val ] : state.first) {
            std::cout << loc << ": " << val << std::endl;
        }
        std::cout << "====================================" << std::endl;

        std::cout << std::endl << "=========== Thread states ==========" << std::endl;
        for (auto& [ thread_id, registers ] : state.second) {
            std::cout << "Thread " << thread_id << std::endl;
            
            for (auto& [ loc, val ] : registers) {
                std::cout << loc << ": " << val << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << "====================================" << std::endl;
    }
    catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}