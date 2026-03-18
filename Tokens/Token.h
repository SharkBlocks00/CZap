//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_TOKEN_H
#define CZAP_TOKEN_H
#include <string>


enum class TokenKind {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    EOFTOKEN,
    STRING,
    BOOLEAN,
    FunctionCall,
    BREAK
};

struct Token {
    TokenKind kind;
    std::string value;
};


#endif //CZAP_TOKEN_H