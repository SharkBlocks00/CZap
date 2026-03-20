#include "Lexer.h"

#include <algorithm>
#include <stdexcept>

Lexer::Lexer()
    : i(0), line_count(-1), keywords {
        "let","output","request","if","else",
        "elseif","func","define","while","break",
        "foreach","const","assert"
    } {}

std::vector<Token> Lexer::lexate(const std::string& source) {
    tokens.clear();
    i = 0;

    while (i < source.length()) {
        const char c = source[i];

        if (c == '\n') {
            line_count ++;
            i++;
            continue;
        }

        if (isspace(c)) {
            i++;
            continue;
        }

        if (c == '-' && i + 1 < source.length() && source[i+1] == '-') {
            while (i < source.length() && source[i] != '\n') {
                i++;
            }
            continue;
        }

        if (c == '"') {
            i++;
            size_t start = i;

            while (i < source.length() && source[i] != '"') {
                i++;
            }

            if (i >= source.length()) {
                throw std::out_of_range("Unterminated string.");
            }

            tokens.push_back({TokenKind::STRING, source.substr(start, i - start)});
            i++;
            continue;
        }

        if (isalpha(c)) {
            size_t start = i;
            while (i < source.length() && (isalnum(source[i]) || source[i] == '_' || source[i] == '-')) {
                i++;
            }

                std::string word = source.substr(start, i - start);

                if (source.substr(start, i - start) == "true"
                    or source.substr(start, i - start) == "false") {
                    tokens.push_back({TokenKind::BOOLEAN, word});
                }

                else if (i < source.size() &&
                         source[i] == '(' &&
                         std::find(keywords.begin(), keywords.end(), word) == keywords.end()) {
                    tokens.push_back({TokenKind::FunctionCall, word});
                }

                else {
                    tokens.push_back({TokenKind::IDENTIFIER, word});
                }
            continue;
        }

        if (isdigit(c)) {
            size_t start = i;
            while (i < source.length() && isdigit(source[i])) {
                i++;
            }
            tokens.push_back({TokenKind::NUMBER, source.substr(start, i - start)});
            continue;
        }

        if (std::string("=;(){}+-*/!<>:&|,%").find(c) != std::string::npos) {
            if (std::string("=!<>").find(c) != std::string::npos && i + 1 < source.length() && source[i + 1] == '=') {
                tokens.push_back({TokenKind::SYMBOL,std::string() + c + '='});
                i += 2;
                continue;
            }
            if (std::string("&|").find(c) != std::string::npos) {
                if (std::string("&|").find(source[i + 1]) != std::string::npos) {
                    tokens.push_back({TokenKind::SYMBOL,std::string() + c + c});
                    i += 2;
                    continue;
                }
            }
            else if (std::string("+-*/%").find(c) != std::string::npos) {
                if (c == '+' && source[i + 1] == '+' || source[i + 1] == '=') {
                    tokens.push_back({TokenKind::SYMBOL, std::string() + c + source[i + 1]});
                    i += 2;
                    continue;
                }
                if (c == '-' && source[i + 1] == '=') {
                    tokens.push_back({TokenKind::SYMBOL, std::string() + c + source[i + 1]});
                    i += 2;
                    continue;
                }
                if (c == '%' && source[i + 1] == '=') {
                    tokens.push_back({TokenKind::SYMBOL, std::string() + c + source[i + 1]});
                    i += 2;
                    continue;
                }
                if (c == '*' && source[i + 1] == '=') {
                    tokens.push_back({TokenKind::SYMBOL, std::string() + c + source[i + 1]});
                    i += 2;
                    continue;
                }
                if (c == '/' && source[i + 1] == '=') {
                    tokens.push_back({TokenKind::SYMBOL, std::string() + c + source[i + 1]});
                    i += 2;
                    continue;
                }
            }

            tokens.push_back({TokenKind::IDENTIFIER, std::string() + c});
            i++;
            continue;
        }
        throw std::out_of_range("Unexpected character: " + std::string() + c);
    }
    std::vector<Token> parsed_tokens = {};

    for (const auto& [kind, value] : tokens) {
        if (kind == TokenKind::STRING) {
            parsed_tokens.emplace_back(TokenKind::STRING, value);
        }
        else if (kind == TokenKind::BOOLEAN) {
            parsed_tokens.emplace_back(TokenKind::BOOLEAN, value);
        }
        else if (kind == TokenKind::IDENTIFIER && value == "break") {
            parsed_tokens.emplace_back(TokenKind::BREAK, value);
        }
        else if (kind == TokenKind::IDENTIFIER && std::find(keywords.begin(), keywords.end(), value) != keywords.end()) {
            parsed_tokens.emplace_back(TokenKind::KEYWORD, value);
        }
        else if (kind == TokenKind::IDENTIFIER) {
            parsed_tokens.emplace_back(TokenKind::IDENTIFIER, value);
        }
        else if (kind == TokenKind::NUMBER) {
            parsed_tokens.emplace_back(TokenKind::NUMBER, value);
        }
        else if (kind == TokenKind::SYMBOL) {
            parsed_tokens.emplace_back(TokenKind::SYMBOL, value);
        }
        else if (kind == TokenKind::FunctionCall) {
            parsed_tokens.emplace_back(TokenKind::FunctionCall, value);
        }
        else {
            throw std::out_of_range("Unexpected token: " + value);
        }
    }
    return parsed_tokens;
}
