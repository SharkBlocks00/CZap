#pragma once

#include "Tokens/Token.h"
#include <memory>
#include <vector>
#include <tuple>


class Statement;
class Expression;

using ASTStatement = std::unique_ptr<Statement>;

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    std::tuple<std::vector<std::unique_ptr<Statement>>, size_t> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    std::tuple<ASTStatement, size_t> parse_statement();
    std::tuple<std::unique_ptr<Expression>, size_t> parse_expression();
    std::tuple<std::unique_ptr<Expression>, size_t> parse_addition();
    std::tuple<std::unique_ptr<Expression>, size_t> parse_multiplication();
    std::tuple<std::unique_ptr<Expression>, size_t> parse_comparison();
    std::tuple<std::unique_ptr<Expression>, size_t> parse_primary();

    std::tuple<ASTStatement, size_t> parse_let();
    std::tuple<ASTStatement, size_t> parse_const();
    std::tuple<ASTStatement, size_t> parse_res();
    std::tuple<ASTStatement, size_t> parse_output();
    std::tuple<std::unique_ptr<Expression>, size_t> parse_request();
    std::tuple<ASTStatement, size_t> parse_while();
    std::tuple<ASTStatement, size_t> parse_assert();
    std::tuple<ASTStatement, size_t> parse_foreach();
    std::tuple<ASTStatement, size_t> parse_if();
    std::tuple<ASTStatement, size_t> parse_function();
    std::tuple<ASTStatement, size_t> parse_function_call();
    std::tuple<ASTStatement, size_t> parse_break();
};