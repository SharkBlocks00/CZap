#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

int main() {
    Lexer lexer;
    auto tokens = lexer.lexate("output(\"Hi\");");

    Parser parser(tokens);

    Interpreter interpreter(lexer, parser);
    interpreter.interpret();

    return 0;

}