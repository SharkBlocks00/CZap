#include <iostream>
#include <filesystem>
#include <fstream>

#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

int main(int argc, char* argv[]) {

    namespace fs = std::filesystem;

    if (argc < 2) {
        std::cout << "Usage: zap <file_path>\n";
        return 1; 
    }

    const std::string file_path = argv[1];
    

    if (!fs::exists(file_path) || !fs::is_regular_file(file_path)) {
        std::cout << "File not found: " << file_path << "\n";
        return 1;
    }

    if (fs::path(file_path).extension() != ".zap") {
        std::cout << "Invalid file type: " << file_path << "\n";
        return 1;
    }

    std::ifstream file(file_path);
    if(!file.is_open()) {
        std::cout << "File not readable: " << file_path << "\n";
        return 1;
    }

    std::string source(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    

    Lexer lexer;
    auto tokens = lexer.lexate(source);

    Parser parser(tokens);

    Interpreter interpreter(lexer, parser);
    interpreter.interpret();

    return 0;

}
