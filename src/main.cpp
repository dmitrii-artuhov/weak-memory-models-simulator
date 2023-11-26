#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "args-parser/args-parser.h"
#include "exceptions/exceptions.h"
#include "file-reader/file-reader.h"
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "interpreter/non-deterministic/non-deterministic.h"
#include "interpreter/tracing/tracing.h"
#include "interpreter/interactive/interactive.h"
#include "interpreter/interactive/interactive-cli.h"
#include "interpreter/model-checking/model-checking.h"
#include "storage-subsystem/sc/sc-storage-subsystem.h"
#include "storage-subsystem/tso/tso-storage-subsystem.h"
#include "storage-subsystem/pso/pso-storage-subsystem.h"

int main([[ maybe_unused ]] int argc, [[ maybe_unused ]] const char* argv[]) {
    using namespace wmm_simulator;
    static const std::string USAGE = "USAGE: " + std::string(argv[0]) + " --file [path/to/program] [--mm [memory model|sc, tso, pso] --it [interpreter type|nd, tr, mc, i]]";

    try {
        if (argc < 2) {
            throw exceptions::invalid_arguments(USAGE);
        }

        struct ProgramOptions {
            std::string program_path;
            std::string memory_model = "sc";
            std::string interpreter_type = "nd";
        };

        auto args_parser = args::CommandOptions<ProgramOptions>::create({
            { "--file", &ProgramOptions::program_path },
            { "--mm", &ProgramOptions::memory_model },
            { "--it", &ProgramOptions::interpreter_type }
        });

        auto opts = args_parser->parse(argc, argv);

        if (opts.program_path.empty()) {
            throw exceptions::invalid_arguments(USAGE);
        }

        if (
            opts.interpreter_type != "nd" &&
            opts.interpreter_type != "tr" &&
            opts.interpreter_type != "mc" &&
            opts.interpreter_type != "i"
        ) {
            throw exceptions::invalid_arguments("Interpreter type must be one of: nd, tr, mc, i");
        }

        if (
            opts.memory_model != "sc" &&
            opts.memory_model != "tso" &&
            opts.memory_model != "pso"
        ) {
            throw exceptions::invalid_arguments("Memory model type must be one of: sc, tso, pso");
        }

        std::cout << "Program path: " << opts.program_path << std::endl;
        std::cout << "Memory model: " << opts.memory_model << std::endl;
        std::cout << "Interpreter type: " << opts.interpreter_type << std::endl;

        std::string code = fs::FileReader::read_file(opts.program_path);

        std::cout << "Program text: \n" << code << std::endl;
        
        Lexer lexer(code.c_str());
        std::vector <Token> tokens = lexer.get_tokens();

        Parser parser(tokens);
        std::pair<
            std::shared_ptr<ProgramNode>,
            std::unordered_map<std::string_view, int>
        > parse_result = parser.parse();


        if (opts.interpreter_type == "nd") {
            NonDeterministicInterpreter interpreter(parse_result.first, parse_result.second);
            if (opts.memory_model == "sc") interpreter.run<SCStorageSubsystem>();
            else if (opts.memory_model == "tso") interpreter.run<TSOStorageSubsystem>();
            else if (opts.memory_model == "pso") interpreter.run<PSOStorageSubsystem>();
        }
        else if (opts.interpreter_type == "tr") {
            TracingInterpreter interpreter(parse_result.first, parse_result.second);
            if (opts.memory_model == "sc") interpreter.run<SCStorageSubsystem>();
            else if (opts.memory_model == "tso") interpreter.run<TSOStorageSubsystem>();
            else if (opts.memory_model == "pso") interpreter.run<PSOStorageSubsystem>();
        }
        else if (opts.interpreter_type == "mc") {
            ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
            if (opts.memory_model == "sc") interpreter.run<SCStorageSubsystem>();
            else if (opts.memory_model == "tso") interpreter.run<TSOStorageSubsystem>();
            else if (opts.memory_model == "pso") interpreter.run<PSOStorageSubsystem>();
        }
        else if (opts.interpreter_type == "i") {
            InteractiveInterpreter interpreter(parse_result.first, parse_result.second);
            if (opts.memory_model == "sc") interpreter.run<SCStorageSubsystem>();
            else if (opts.memory_model == "tso") interpreter.run<TSOStorageSubsystem>();
            else if (opts.memory_model == "pso") interpreter.run<PSOStorageSubsystem>();
        }
    }
    catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}