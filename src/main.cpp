#include <iostream>

#include "exceptions/exceptions.h"
#include "file-reader/file-reader.h"

int main(int argc, char* argv[]) {
    using namespace wmm_simulator;
    
    try {
        if (argc != 2) {
            throw exceptions::invalid_arguments("USAGE: " + std::string(argv[0]) + " [path/to/program]");
        }
    
        std::string program_path = argv[1];
        std::string program_text = fs::FileReader::read_file(program_path);

        std::cout << "Program text: \n" << program_text << std::endl;

        
    }
    catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}