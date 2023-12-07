#include "doctest.h"
#include "common.h"

#include <iostream>
#include <string>
#include <unordered_map>

#include "file-reader/file-reader.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/model-checking/model-checking.h"
#include "storage-subsystem/sra/sra-storage-subsystem.h"


namespace doctest {
using namespace wmm_simulator;

TEST_CASE("SRA: single-thread") {
    std::string path = test_data_path + "/single-thread.txt";
    std::string code = fs::FileReader::read_file(path);

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());
    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > parse_result = parser.parse();

    ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
    auto states = interpreter.run<SRAStorageSubsystem>();

    CHECK(states.size() == 1);
    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, create_thread_subsystem({ { "a", 3 }, { "r", 3 } }) }
        })
    ));
}

TEST_CASE("SRA: store-buffering") {
    std::string path = test_data_path + "/store-buffering.txt";
    std::string code = fs::FileReader::read_file(path);

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());
    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > parse_result = parser.parse();

    ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
    auto states = interpreter.run<SRAStorageSubsystem>();

    CHECK(states.size() == 4);

    auto ts1_0 = create_thread_subsystem({ { "a", 0 }, { "r", 1 } });
    auto ts1_1 = create_thread_subsystem({ { "a", 1 }, { "r", 1 } });
    
    auto ts2_0 = create_thread_subsystem({ { "b", 0 }, { "r", 1 } });
    auto ts2_1 = create_thread_subsystem({ { "b", 1 }, { "r", 1 } });

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_0 },
            { 2, ts2_1 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_1 },
            { 2, ts2_0 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_1 },
            { 2, ts2_1 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_0 },
            { 2, ts2_0 }
        })
    ));
}


TEST_CASE("SRA: message-passing") {
    std::string path = test_data_path + "/message-passing.txt";
    std::string code = fs::FileReader::read_file(path);

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());
    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > parse_result = parser.parse();

    ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
    auto states = interpreter.run<SRAStorageSubsystem>();

    CHECK(states.size() == 4);

    auto ts1 = create_thread_subsystem({ { "r", 1 } });
    
    auto ts2_00 = create_thread_subsystem({ { "a", 0 }, { "b", 0 } });
    auto ts2_01 = create_thread_subsystem({ { "a", 0 }, { "b", 1 } });
    auto ts2_10 = create_thread_subsystem({ { "a", 1 }, { "b", 0 } });
    auto ts2_11 = create_thread_subsystem({ { "a", 1 }, { "b", 1 } });
    
    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_00 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_01 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_10 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_11 }
        })
    ));
}


TEST_CASE("SRA: message-passing-strong-rel-acq") {
    std::string path = test_data_path + "/message-passing-strong-rel-acq.txt";
    std::string code = fs::FileReader::read_file(path);

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());
    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > parse_result = parser.parse();

    ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
    auto states = interpreter.run<SRAStorageSubsystem>();

    CHECK(states.size() == 3);

    auto ts1 = create_thread_subsystem({ { "r", 1 } });
    
    auto ts2_00 = create_thread_subsystem({ { "a", 0 }, { "b", 0 } });
    auto ts2_01 = create_thread_subsystem({ { "a", 0 }, { "b", 1 } });
    auto ts2_11 = create_thread_subsystem({ { "a", 1 }, { "b", 1 } });
    
    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_00 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_01 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1 },
            { 2, ts2_11 }
        })
    ));
}

TEST_CASE("SC: load-buffering") {
    std::string path = test_data_path + "/load-buffering.txt";
    std::string code = fs::FileReader::read_file(path);

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());
    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > parse_result = parser.parse();

    ModelCheckingInterpreter interpreter(parse_result.first, parse_result.second);
    auto states = interpreter.run<SRAStorageSubsystem>();

    CHECK(states.size() == 3);

    auto ts1_0 = create_thread_subsystem({ { "a", 0 }, { "r", 1 } });
    auto ts1_1 = create_thread_subsystem({ { "a", 1 }, { "r", 1 } });
    
    auto ts2_0 = create_thread_subsystem({ { "b", 0 }, { "r", 1 } });
    auto ts2_1 = create_thread_subsystem({ { "b", 1 }, { "r", 1 } });
    
    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_0 },
            { 2, ts2_1 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_1 },
            { 2, ts2_0 }
        })
    ));

    CHECK(has_threads_subsystems(
        states,
        std::unordered_map<int, ThreadSubsystem>({
            { 0, ThreadSubsystem() },
            { 1, ts1_0 },
            { 2, ts2_0 }
        })
    ));
}

}
