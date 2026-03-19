//
// Created by ogkbe on 3/19/2026.
//

#include "Token.h"

std::ostream& operator<<(std::ostream& os, const TokenKind& kind) {
    switch (kind) {
        case TokenKind::KEYWORD:      return os << "KEYWORD";
        case TokenKind::IDENTIFIER:   return os << "IDENTIFIER";
        case TokenKind::NUMBER:       return os << "NUMBER";
        case TokenKind::SYMBOL:       return os << "SYMBOL";
        case TokenKind::EOFTOKEN:     return os << "EOF";
        case TokenKind::STRING:       return os << "STRING";
        case TokenKind::BOOLEAN:      return os << "BOOLEAN";
        case TokenKind::FunctionCall: return os << "FunctionCall";
        case TokenKind::BREAK:        return os << "BREAK";
    }
    return os << "UNKNOWN";
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << token.kind << ", " << token.value << ")";
    return os;
}
