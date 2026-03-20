
#ifndef CZAP_LEXER_H
#define CZAP_LEXER_H
#include <string>
#include <vector>

#include "Tokens/Token.h"


class Lexer {
public:
    Lexer();

    std::vector<Token> lexate(const std::string& source);

private:
    size_t i;
    std::vector<Token> tokens;
    int line_count;

    std::vector<std::string> keywords;
};


#endif //CZAP_LEXER_H