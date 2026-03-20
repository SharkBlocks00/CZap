#include "Parser.h"

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

#include <stdexcept>
#include <iostream>


std::tuple<ASTStatement, size_t> Parser::parse_statement() {
    auto token = tokens[pos];
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
            auto [stmt, new_pos] = parse_request();
            node = std::move(stmt);
            pos = new_pos;
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
        if (tokens[pos + 1].kind == TokenKind::SYMBOL && tokens[pos + 1].value == "(") {
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
    auto [left, pos] = parse_multiplication();

    while (pos < tokens.size() && tokens[pos].kind == TokenKind::SYMBOL && (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        std::string op = tokens[pos].value;
        pos++;

        auto [right, pos] = parse_multiplication();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }

    return {std::move(left), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_multiplication() {
    auto [left, pos] = parse_primary();

    while (pos < tokens.size() && tokens[pos].kind == TokenKind::SYMBOL && (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        std::string op = tokens[pos].value;
        pos++;
        auto [right, pos] = parse_primary();
        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }

    return {std::move(left), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_comparison() {
    auto [left, pos] = parse_addition();

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
            auto [right, pos] = parse_addition();
            left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return {std::move(left), pos};
}

std::tuple<std::unique_ptr<Expression>, size_t> Parser::parse_primary() {
    auto token = tokens[pos];

    if (token.kind == TokenKind::NUMBER) {
        auto value = std::stoi(token.value);
        pos++;
        
        std::unique_ptr<Expression> node = std::make_unique<NumberLiteral>(value);

        return std::make_tuple(std::move(node), pos);
    }

    throw std::runtime_error("Unexpected token in primary expression: " + token.value);
}

std::tuple<ASTStatement, size_t> Parser::parse_let() {
    auto token = tokens[pos];
    
    if (token.kind != TokenKind::KEYWORD || token.value != "let") {
        throw std::runtime_error("Expected 'let' keyword to define a variable, got: " + token.value);
    }

    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after 'let', got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after variable name in 'let' declaration, got: " + token.value);
    }
    pos++;
    auto [value, pos] = parse_expression();
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after variable declaration, got: " + token.value);
    }
    pos++;

    return std::make_tuple(std::make_unique<VarDeclaration>(std::move(name), std::move(value)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_const() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "const") {
        throw std::runtime_error("Expected 'const' keyword to define a constant variable, got: " + token.value);
    }

    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after 'const', got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after variable name in 'const' declaration, got: " + token.value);
    }

    auto [value, pos] = parse_expression();
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after constant variable declaration, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<VarDeclaration>(std::move(name), std::move(value), false), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_res() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier for variable reassignment, got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after variable name in reassignment, got: " + token.value);
    }

    pos++;
    auto [value, pos] = parse_expression();
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after variable reassignment, got: " + token.value);
    }

    pos++;
    return std::make_tuple(std::make_unique<VarReassignment>(std::move(name), std::move(value)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_output() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "output") {
        throw std::runtime_error("Expected 'output' keyword, got: " + token.value);
    }

    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'output', got: " + token.value);
    }

    pos++;
    token = tokens[pos];

    auto [value, pos] = parse_expression();
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after output expression, got: " + token.value);
    }
    pos++;
    token = tokens[pos];


    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after output statement, got: " + token.value);
    }

    pos++;
    return std::make_tuple(std::make_unique<OutputStatement>(std::move(value)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_request() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "request") {
        throw std::runtime_error("Expected 'request' keyword, got: " + token.value);
    }

    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'request', got: " + token.value);
    }

    pos++;
    token = tokens[pos];
    auto [prompt, pos] = parse_expression();

    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after request expression, got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after request statement, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<RequestStatement>(std::move(prompt)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_while() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "while") {
        throw std::runtime_error("Expected 'while' keyword, got: " + token.value);
    }

    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'while', got: " + token.value);
    }

    pos++;

    auto [condition, pos] = parse_expression();
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after while condition, got: " + token.value);
    }
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start while loop body, got: " + token.value);
    }

    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = tokens[pos];

    while (token.value != "}") {
        auto [stmt, pos] = parse_statement();
        body.push_back(std::move(stmt));
        token = tokens[pos];
    }
    pos++;
    return std::make_tuple(std::make_unique<WhileLoop>(std::move(condition), std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_assert() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "assert") {
        throw std::runtime_error("Expected 'assert' keyword, got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'assert', got: " + token.value);
    }
    pos++;
    auto [condition, pos] = parse_expression();
    token = tokens[pos];
    std::string message;
    if (token.kind != TokenKind::SYMBOL || token.value != ",") {
        pos++;
        message = "";
    } else {
        pos++;
        token = tokens[pos];
        if (token.kind != TokenKind::STRING) {
            throw std::runtime_error("Expected string after ',' in 'assert', got: " + token.value);
        }
        message = token.value;
        pos++;
        token = tokens[pos];
    }
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        if (token.kind == TokenKind::SYMBOL && token.value == ";") {
            return std::make_tuple(std::make_unique<AssertStatement>(std::move(condition)), pos);
        }
        throw std::runtime_error("Expected ')' after assert condition, got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after assert statement, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<AssertStatement>(std::move(condition), std::move(message)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_foreach() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "foreach") {
        throw std::runtime_error("Expected 'foreach' keyword, got: " + token.value);
    }

    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'foreach', got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    std::string varName = token.value;
    pos++;

    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ":") {
        throw std::runtime_error("Expected ':' after variable name in 'foreach', got: " + token.value);
    }

    pos++;
    auto [collection, pos] = parse_expression();
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after foreach collection expression, got: " + token.value);
    }
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start foreach loop body, got: " + token.value);
    }
    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = tokens[pos];

    while (token.value != "}") {
        auto [stmt, pos] = parse_statement();
        body.push_back(std::move(stmt));
        token = tokens[pos];
    }

    pos++;
    return std::make_tuple(std::make_unique<ForeachLoop>(std::move(varName), std::move(collection), std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_if() {
    auto token = tokens[pos];

    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'if', got: " + token.value);
    }  

    pos++;
    auto [condition, pos] = parse_expression();

    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after if condition, got: " + token.value);
    }
    pos++;

    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start if statement body, got: " + token.value);
    }
    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = tokens[pos];

    while (token.value != "}") {
        auto [stmt, pos] = parse_statement();
        body.push_back(std::move(stmt));
        token = tokens[pos];
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
        token = tokens[pos];

        if (token.kind != TokenKind::SYMBOL || token.value != "{") {
            throw std::runtime_error("Expected '{' to start else statement body, got: " + token.value);
        }
        pos++;

        std::vector<std::unique_ptr<Statement>> elseBody = {};
        token = tokens[pos];

        while (token.value != "}") {
            auto [stmt, pos] = parse_statement();
            elseBody.push_back(std::move(stmt));
            token = tokens[pos];
        }
        pos++;
        return std::make_tuple(std::make_unique<IfStatement>(std::move(condition), std::move(body), std::move(elseBody)), pos);
    }
    return std::make_tuple(std::make_unique<IfStatement>(std::move(condition), std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_function() {
    auto token = tokens[pos];

    if (token.kind != TokenKind::KEYWORD || token.value != "func") {
        throw std::runtime_error("Expected 'func' keyword to define a function, got: " + token.value);
    }

    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected function name after 'func', got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "=") {
        throw std::runtime_error("Expected '=' after function name, got: " + token.value);
    }
    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::KEYWORD || token.value != "define") {
        throw std::runtime_error("Expected 'define' keyword after function name and '=', got: " + token.value);
    }

    pos++;
    token = tokens[pos];

    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after 'define' in function declaration, got: " + token.value);
    }
    pos++;
    // TODO: allow for parameters
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after 'define' in function declaration, got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != "{") {
        throw std::runtime_error("Expected '{' to start function body, got: " + token.value);
    }
    pos++;
    std::vector<std::unique_ptr<Statement>> body = {};
    token = tokens[pos];
    
    while (token.value != "}") {
        auto [stmt, pos] = parse_statement();
        body.push_back(std::move(stmt));
        token = tokens[pos];
    }
    pos++;
    return std::make_tuple(std::make_unique<Function>(std::move(name), std::vector<std::string>{}, std::move(body)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_function_call() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected function name for function call, got: " + token.value);
    }

    std::string name = token.value;
    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != "(") {
        throw std::runtime_error("Expected '(' after function name in function call, got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    // TODO: allow for arguments
    if (token.kind != TokenKind::SYMBOL || token.value != ")") {
        throw std::runtime_error("Expected ')' after function name in function call, got: " + token.value);
    }
    pos++;
    token = tokens[pos];
    if (token.kind != TokenKind::SYMBOL || token.value != ";") {
        throw std::runtime_error("Expected ';' after function call, got: " + token.value);
    }
    pos++;
    return std::make_tuple(std::make_unique<FunctionCall>(std::move(name)), pos);
}

std::tuple<ASTStatement, size_t> Parser::parse_break() {
    auto token = tokens[pos];
    if (token.kind != TokenKind::KEYWORD || token.value != "break") {
        throw std::runtime_error("Expected 'break' keyword, got: " + token.value);
    }

    pos++;
    token = tokens[pos];
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
