#pragma once

#include "Lexer.h"
#include "Parser.h"
#include "Environment/Environment.h"
#include "Helpers/Value.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

class Statement;
class Expression;
class Function;

class Interpreter {
public:
    Interpreter(Lexer lexer, Parser parser);
    void interpret();

private:
    Lexer lexer;
    Parser parser;
    std::shared_ptr<Environment> global_environment;
    std::unordered_map<std::string, const Function*> functions;

    Value convert_input(const std::string& value);
    Value eval_expression(const Expression* expr, const std::shared_ptr<Environment>& environment);
    bool interpret_nodes(const std::vector<std::unique_ptr<Statement>>& nodes,
        const std::shared_ptr<Environment>& environment);
    bool is_truthy(const Value& value) const;
    std::string value_to_string(const Value& value) const;
    double value_to_number(const Value& value);
};
