#include "Parser.h"

#include "ASTNodes/ExpressionStatement.h"
#include "ASTNodes/StringLiteral.h"
#include "ASTNodes/VarDeclaration.h"
#include "ASTNodes/VarReassignment.h"
#include "ASTNodes/OutputStatement.h"
#include "ASTNodes/RequestStatement.h"
#include "ASTNodes/IfStatement.h"
#include "ASTNodes/WhileLoop.h"
#include "ASTNodes/AssertStatement.h"
#include "ASTNodes/ForeachLoop.h"
#include "ASTNodes/Function.h"
#include "ASTNodes/FunctionCall.h"
#include "ASTNodes/BreakStatement.h"
#include "ASTNodes/BinaryExpression.h"
#include "ASTNodes/NumberLiteral.h"
#include "ASTNodes/BooleanLiteral.h"
#include "ASTNodes/Identifier.h"


#include <stdexcept>
#include <iostream>

namespace {
const Token& token_at(const std::vector<Token>& tokens, size_t pos, const std::string& context) {
    if (pos >= tokens.size()) {
        throw std::runtime_error("Unexpected end of input while parsing " + context);
    }
    return tokens[pos];
}
}

std::tuple<ASTStatement, size_t> Parser::parse_statement() {
    auto token = token_at(tokens, pos, "statement");
    ASTStatement node = nullptr;

    if (token.kind == TokenKind::KEYWORD) {
        if (token.value == "let") {
            auto [stmt, new_pos] = parse_let();
            node = std::move(stmt);
            pos = new_pos;
        } 
        else if (token.value == "const") {
            auto [stmt, new_pos] = parse_const();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "output") {
            auto [stmt, new_pos] = parse_output();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "request") {
            auto [expression, new_pos] = parse_request();
            pos = new_pos;
            node = std::make_unique<ExpressionStatement>(std::move(expression));
        }
        else if (token.value == "while") {
            auto [stmt, new_pos] = parse_while();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "assert") {
            auto [stmt, new_pos] = parse_assert();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "foreach") {
            auto [stmt, new_pos] = parse_foreach();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "if") {
            auto [stmt, new_pos] = parse_if();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "elseif") {
            throw std::runtime_error("Unexpected 'elseif' without preceding 'if'");
        }
        else if (token.value == "else") {
            throw std::runtime_error("Unexpected 'else' without preceding 'if'");
        }
        else if (token.value == "func") {
            auto [stmt, new_pos] = parse_function();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "break") {
            auto [stmt, new_pos] = parse_break();
            node = std::move(stmt);
            pos = new_pos;
        }
        else if (token.value == "define") {
            throw std::runtime_error("The 'define' keyword is used after declaring a function");
        }
        else {
            throw std::runtime_error("Unexpected keyword: " + token.value);
        }
    }
    else if (token.kind == TokenKind::IDENTIFIER) {
        if (pos + 1 < tokens.size() && tokens[pos + 1].kind == TokenKind::SYMBOL && tokens[pos + 1].value == "(") {
            auto [stmt, new_pos] = parse_function_call();
            node = std::move(stmt);
            pos = new_pos;
        }
        else {
            auto [stmt, new_pos] = parse_res();
            node = std::move(stmt);
            pos = new_pos;
        }
    }
    else {
        throw std::runtime_error("Unexpected token: " + token.value);
    
    }
    return {std::move(node), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_expression() {
    return parse_comparison();
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_addition() {
    auto [left, new_pos] = parse_multiplication();
    pos = new_pos;

    while (pos < tokens.size() && tokens[pos].kind == TokenKind::SYMBOL && (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        std::string op = tokens[pos].value;
        pos++;

        auto [right, new_pos] = parse_multiplication();
        pos = new_pos;

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }

    return {std::move(left), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_multiplication() {
    auto [left, new_pos] = parse_primary();
    pos = new_pos;

    while (pos < tokens.size() && tokens[pos].kind == TokenKind::SYMBOL && (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        std::string op = tokens[pos].value;
        pos++;
        auto [right, new_pos] = parse_primary();
        pos = new_pos;
        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }

    return {std::move(left), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_comparison() {
    auto [left, new_pos] = parse_addition();
    pos = new_pos;

    while (pos < tokens.size() && tokens[pos].kind == TokenKind::SYMBOL && (
        tokens[pos].value == "==" ||
        tokens[pos].value == "!=" ||
        tokens[pos].value == "<" ||
        tokens[pos].value == "<=" ||
        tokens[pos].value == ">" ||
        tokens[pos].value == ">=" ||
        tokens[pos].value == "&&" ||
        tokens[pos].value == "||" )) {

            std::string op = tokens[pos].value;
            pos++;
            auto [right, new_pos] = parse_addition();
            pos = new_pos;
            left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return {std::move(left), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_primary() {
    auto token = token_at(tokens, pos, "primary expression");

    if (token.kind == TokenKind::NUMBER) {
        auto value = std::stoi(token.value);
        pos++;
        
        std::unique_ptr<Expression> node = std::make_unique<NumberLiteral>(value);

        return std::make_tuple(std::move(node), pos);
    }
    if (token.kind == TokenKind::BOOLEAN) {
        auto value = token.value == "true";
        pos++;

        std::unique_ptr<Expression> node = std::make_unique<BooleanLiteral>(value);

        return std::make_tuple(std::move(node), pos);
    }
    if (token.kind == TokenKind::STRING) {
        auto value = token.value;
        pos++;

        std::unique_ptr<Expression> node = std::make_unique<StringLiteral>(std::move(value));

        return std::make_tuple(std::move(node), pos);
    }
    if (token.kind == TokenKind::IDENTIFIER) {
        auto value = token.value;
        pos++;

        std::unique_ptr<Expression> node = std::make_unique<Identifier>(std::move(value));

        return std::make_tuple(std::move(node), pos);
    }
    if (token.kind == TokenKind::KEYWORD && token.value == "request") {
        return parse_request();
    }
    if (token.kind == TokenKind::SYMBOL && token.value == "(") {
        pos++;
        auto [expression, new_pos] = parse_expression();
        
        pos = new_pos;

        const auto& closing = token_at(tokens, pos, "grouped expression");
        if (closing.kind != TokenKind::SYMBOL || closing.value != ")") {
            throw std::runtime_error("Expected ')' after expression, got: " + closing.value);
        }

        return std::make_tuple(std::move(expression), pos + 1);
    }

    std::cout << token.kind << std::endl;
    throw std::runtime_error("Unexpected token in primary expression: " + token.value );
}

std::tuple<ASTStatement, size_t> Parser::parse_let() {
    auto token = token_at(tokens, pos, "'let' declaration");
    
    if (token.kind != TokenKind::KEYWORD || token.value != "let") {
        throw std::runtime_error("Expected 'let' keyword to define a variable, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "'let' declaration");
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after 'let', got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = token_at(tokens, pos, "'let' declaration");

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after variable name in 'let' declaration, got: " + token.value);
    }
    pos++;
    auto [value, new_pos] = parse_expression();
    pos = new_pos;
    token = token_at(tokens, pos, "'let' declaration");

    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after variable declaration, got: " + token.value);
    }
    pos++;

    return std::make_tuple(std::make_unique<VarDeclaration>(std::move(name), std::move(value)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_const() {
    auto token = token_at(tokens, pos, "'const' declaration");
    if (token.kind != TokenKind::KEYWORD || token.value != "const") {
        throw std::runtime_error("Expected 'const' keyword to define a constant variable, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "'const' declaration");

    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after 'const', got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = token_at(tokens, pos, "'const' declaration");

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after variable name in 'const' declaration, got: " + token.value);
    }

    pos++;
    auto [value, new_pos] = parse_expression();
    pos = new_pos;
    token = token_at(tokens, pos, "'const' declaration");

    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after constant variable declaration, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<VarDeclaration>(std::move(name), std::move(value), false), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_res() {
    auto token = token_at(tokens, pos, "reassignment");
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier for variable reassignment, got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = token_at(tokens, pos, "reassignment");

    if (token.kind != TokenKind::SYMBOL) {
        throw std::runtime_error("Expected assignment operator after variable name in reassignment, got: " + token.value);
    }
    std::optional<std::string> op = std::nullopt;
    if (token.value == "=") {
        op = std::nullopt;
    } else if (token.value == "+=" || token.value == "-=" || token.value == "*=" || token.value == "/=" || token.value == "%=") {
        op = token.value;
    } else {
        throw std::runtime_error("Expected assignment operator (=, +=, -=, *=, /=, %=), got: " + token.value);
    }

    pos++;
    auto [value, new_pos] = parse_expression();
    pos = new_pos;
    token = token_at(tokens, pos, "reassignment");

    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after variable reassignment, got: " + token.value);
    }

    pos++;
    return std::make_tuple(std::make_unique<VarReassignment>(std::move(name), std::move(value), std::move(op)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_output() {
    auto token = token_at(tokens, pos, "output statement");
    if (token.kind != TokenKind::KEYWORD || token.value != "output") {
        throw std::runtime_error("Expected 'output' keyword, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "output statement");

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        std::cout << token.value << std::endl;
        std::cout << token.kind << std::endl;
        throw std::runtime_error("Expected '(' after 'output', got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "output statement");

    auto [value, new_pos] = parse_expression();
    pos = new_pos;
    token = token_at(tokens, pos, "output statement");

    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after output expression, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "output statement");


    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after output statement, got: " + token.value);
    }

    pos++;
    return std::make_tuple(std::make_unique<OutputStatement>(std::move(value)), pos);
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_request() {
    auto token = token_at(tokens, pos, "request expression");
    if (token.kind != TokenKind::KEYWORD || token.value != "request") {
        throw std::runtime_error("Expected 'request' keyword, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "request expression");

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'request', got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "request expression");
    auto [prompt, new_pos] = parse_expression();
    pos = new_pos;

    token = token_at(tokens, pos, "request expression");
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after request expression, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "request expression");
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after request statement, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<RequestStatement>(std::move(prompt)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_while() {
    auto token = token_at(tokens, pos, "while loop");
    if (token.kind != TokenKind::KEYWORD || token.value != "while") {
        throw std::runtime_error("Expected 'while' keyword, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "while loop");

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'while', got: " + token.value);
    }

    pos++;

    auto [condition, new_pos] = parse_expression();
    pos = new_pos;
    token = token_at(tokens, pos, "while loop");

    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after while condition, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "while loop");

    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start while loop body, got: " + token.value);
    }

    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = token_at(tokens, pos, "while loop body");

    while (pos < tokens.size() && token.value != "}") {
        auto [stmt, parsed_pos] = parse_statement();
        body.push_back(std::move(stmt));
        pos = parsed_pos;
        token = token_at(tokens, pos, "while loop body");
    }
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected '}' to close while loop body");
    }
    pos++;
    return std::make_tuple(std::make_unique<WhileLoop>(std::move(condition), std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_assert() {
    auto token = token_at(tokens, pos, "assert statement");
    if (token.kind != TokenKind::KEYWORD || token.value != "assert") {
        throw std::runtime_error("Expected 'assert' keyword, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "assert statement");
    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'assert', got: " + token.value);
    }
    pos++;
    auto [condition, new_pos] = parse_expression();
    pos = new_pos;
    token = token_at(tokens, pos, "assert statement");
    std::optional<std::string> message = std::nullopt;
    if (token.kind == TokenKind::SYMBOL && token.value == ",") {
        pos++;
        token = token_at(tokens, pos, "assert statement");
        if (token.kind != TokenKind::STRING) {
            throw std::runtime_error("Expected string after ',' in 'assert', got: " + token.value);
        }
        message = token.value;
        pos++;
    }
    token = token_at(tokens, pos, "assert statement");
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after assert condition, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "assert statement");
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after assert statement, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<AssertStatement>(std::move(condition), std::move(message)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_foreach() {
    auto token = token_at(tokens, pos, "foreach loop");
    if (token.kind != TokenKind::KEYWORD || token.value != "foreach") {
        throw std::runtime_error("Expected 'foreach' keyword, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "foreach loop");
    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'foreach', got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "foreach loop");
    std::string varName = token.value;
    pos++;

    token = token_at(tokens, pos, "foreach loop");
    if (token.kind != TokenKind::SYMBOL || token.value != ":") {
        throw std::runtime_error("Expected ':' after variable name in 'foreach', got: " + token.value);
    }

    pos++;
    auto [collection, parsed_pos] = parse_expression();
    pos = parsed_pos;
    token = token_at(tokens, pos, "foreach loop");

    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after foreach collection expression, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "foreach loop");

    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start foreach loop body, got: " + token.value);
    }
    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = token_at(tokens, pos, "foreach loop body");

    while (pos < tokens.size() && token.value != "}") {
        auto [stmt, parsed_pos] = parse_statement();
        body.push_back(std::move(stmt));
        pos = parsed_pos;
        token = token_at(tokens, pos, "foreach loop body");
    }
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected '}' to close foreach loop body");
    }

    pos++;
    return std::make_tuple(std::make_unique<ForeachLoop>(std::move(varName), std::move(collection), std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_if() {
    auto token = token_at(tokens, pos, "if statement");

    pos++;
    token = token_at(tokens, pos, "if statement");

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'if', got: " + token.value);
    }  

    pos++;
    auto [condition, parsed_pos] = parse_expression();
    pos = parsed_pos;

    token = token_at(tokens, pos, "if statement");
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after if condition, got: " + token.value);
    }
    pos++;

    token = token_at(tokens, pos, "if statement");
    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start if statement body, got: " + token.value);
    }
    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = token_at(tokens, pos, "if body");

    while (pos < tokens.size() && token.value != "}") {
        auto [stmt, parsed_pos] = parse_statement();
        body.push_back(std::move(stmt));
        pos = parsed_pos;
        token = token_at(tokens, pos, "if body");
    }
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected '}' to close if body");
    }
    pos++;

    std::vector<std::unique_ptr<Statement>> elseBody = {};

    if (pos < tokens.size() && tokens[pos].kind == TokenKind::KEYWORD && tokens[pos].value == "elseif") {
        auto [else_stmt, new_pos] = parse_if();
        elseBody.push_back(std::move(else_stmt));
        return std::make_tuple(std::make_unique<IfStatement>(std::move(condition), std::move(body), std::move(elseBody)), new_pos);
    }
    else if (pos < tokens.size() && tokens[pos].kind == TokenKind::KEYWORD && tokens[pos].value == "else") {
        pos++;
        token = token_at(tokens, pos, "else body");

        if (token.kind != TokenKind::SYMBOL || token.value != "{") {
            throw std::runtime_error("Expected '{' to start else statement body, got: " + token.value);
        }
        pos++;

        std::vector<std::unique_ptr<Statement>> elseBody = {};
        token = token_at(tokens, pos, "else body");

        while (pos < tokens.size() && token.value != "}") {
            auto [stmt, parsed_pos] = parse_statement();
            elseBody.push_back(std::move(stmt));
            pos = parsed_pos;
            token = token_at(tokens, pos, "else body");
        }
        if (pos >= tokens.size()) {
            throw std::runtime_error("Expected '}' to close else body");
        }
        pos++;
        return std::make_tuple(std::make_unique<IfStatement>(std::move(condition), std::move(body), std::move(elseBody)), pos);
    }
    return std::make_tuple(std::make_unique<IfStatement>(std::move(condition), std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_function() {
    auto token = token_at(tokens, pos, "function declaration");

    if (token.kind != TokenKind::KEYWORD || token.value != "func") {
        throw std::runtime_error("Expected 'func' keyword to define a function, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "function declaration");
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected function name after 'func', got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = token_at(tokens, pos, "function declaration");

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after function name, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "function declaration");

    if (token.kind != TokenKind::KEYWORD || token.value != "define") {
        throw std::runtime_error("Expected 'define' keyword after function name and '=', got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "function declaration");

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'define' in function declaration, got: " + token.value);
    }
    pos++;
    // TODO: allow for parameters
    token = token_at(tokens, pos, "function declaration");
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after 'define' in function declaration, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "function declaration");
    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start function body, got: " + token.value);
    }
    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = token_at(tokens, pos, "function body");
    
    while (pos < tokens.size() && token.value != "}") {
        auto [stmt, parsed_pos] = parse_statement();
        body.push_back(std::move(stmt));
        pos = parsed_pos;
        token = token_at(tokens, pos, "function body");
    }
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected '}' to close function body");
    }
    pos++;
    return std::make_tuple(std::make_unique<Function>(std::move(name), std::vector<std::string>{}, std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_function_call() {
    auto token = token_at(tokens, pos, "function call");
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected function name for function call, got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = token_at(tokens, pos, "function call");
    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after function name in function call, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "function call");
    // TODO: allow for arguments
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after function name in function call, got: " + token.value);
    }
    pos++;
    token = token_at(tokens, pos, "function call");
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after function call, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<FunctionCall>(std::move(name)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_break() {
    auto token = token_at(tokens, pos, "break statement");
    if (token.kind != TokenKind::KEYWORD || token.value != "break") {
        throw std::runtime_error("Expected 'break' keyword, got: " + token.value);
    }

    pos++;
    token = token_at(tokens, pos, "break statement");
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after 'break' statement, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<BreakStatement>(), pos);
}

std::tuple<std::vector<std::unique_ptr<Statement>>, size_t> Parser::parse() {
    std::vector<ASTStatement> nodes = {};
    while (pos < tokens.size()) {
        auto [stmt, new_pos] = parse_statement();
        nodes.push_back(std::move(stmt));
        this->pos = new_pos;
    }
    return {std::move(nodes), pos};
}
