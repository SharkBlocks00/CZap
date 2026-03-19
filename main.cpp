#include <iostream>
#include <ostream>

#include "Lexer.h"

int main() {
    Lexer lexer;
    auto tokens = lexer.lexate("let bob = x;");

    for (const auto& token : tokens) {
        std::cout << token << std::endl;
    }
}