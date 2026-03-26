#include "Interpreter.h"

#include "ASTNodes/AssertStatement.h"
#include "ASTNodes/BinaryExpression.h"
#include "ASTNodes/BooleanExpression.h"
#include "ASTNodes/BooleanLiteral.h"
#include "ASTNodes/BreakStatement.h"
#include "ASTNodes/ForeachLoop.h"
#include "ASTNodes/Function.h"
#include "ASTNodes/FunctionCall.h"
#include "ASTNodes/Identifier.h"
#include "ASTNodes/IfStatement.h"
#include "ASTNodes/NumberLiteral.h"
#include "ASTNodes/OutputStatement.h"
#include "ASTNodes/RequestStatement.h"
#include "ASTNodes/StringLiteral.h"
#include "ASTNodes/VarDeclaration.h"
#include "ASTNodes/VarReassignment.h"
#include "ASTNodes/WhileLoop.h"

#include <iostream>
#include <stdexcept>
#include <cctype>
#include <unordered_set>

Interpreter::Interpreter(Lexer lexer, Parser parser)
    : lexer(std::move(lexer)),
    parser(std::move(parser)),
    global_environment(std::make_shared<Environment>()) {}

void Interpreter::interpret() {
    auto [nodes, _] = parser.parse();
    interpret_nodes(nodes, global_environment);
}

Value Interpreter::convert_input(const std::string& value) {
    if (!value.empty()) {
        bool all_digits = true;
        for (const char c : value) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return std::stoi(value);
        }
    }

    try {
        size_t idx = 0;
        double parsed = std::stod(value, &idx);
        if (idx == value.size()) {
            return parsed;
        }
    } catch (const std::exception&) {
    }
    return value;
}

Value Interpreter::eval_expression(const Expression* expr, const std::shared_ptr<Environment>& environment) {
    if (!expr) {
        throw std::runtime_error("Null expression");
    }

    if (const auto* number = dynamic_cast<const NumberLiteral*>(expr)) {
        return number->value;
    }
    if (const auto* stringLiteral = dynamic_cast<const StringLiteral*>(expr)) {
        return stringLiteral->value;
    }
    if (const auto* booleanLiteral = dynamic_cast<const BooleanLiteral*>(expr)) {
        return booleanLiteral->value;
    }
    if (const auto* identifier = dynamic_cast<const Identifier*>(expr)) {
        return environment->get(identifier->value);
    }
    if (const auto* request = dynamic_cast<const RequestStatement*>(expr)) {
        Value promptValue = eval_expression(request->value.get(), environment);
        std::cout << value_to_string(promptValue);
        std::string input;
        std::getline(std::cin, input);
        return convert_input(input);
    }
    if (const auto* booleanExpression = dynamic_cast<const BooleanExpression*>(expr)) {
        Value leftValue = eval_expression(booleanExpression->left.get(), environment);
        Value rightValue = eval_expression(booleanExpression->right.get(), environment);

        if (booleanExpression->op == "&&") {
            return is_truthy(leftValue) && is_truthy(rightValue);
        }
        if (booleanExpression->op == "||") {
            return is_truthy(leftValue) || is_truthy(rightValue);
        }

        double left = value_to_number(leftValue);
        double right = value_to_number(rightValue);

        if (booleanExpression->op == "==") {
            return left == right;
        }
        if (booleanExpression->op == "!=") {
            return left != right;
        }

        if (booleanExpression->op == "<") {
            return left < right;
        }
        if (booleanExpression->op == ">") {
            return left > right;
        }
        if (booleanExpression->op == "<=") {
            return left <= right;
        }
        if (booleanExpression->op == ">=") {
            return left >= right;
        }
    }
    if (const auto* binary = dynamic_cast<const BinaryExpression*>(expr)) {
        Value leftValue = eval_expression(binary->left.get(), environment);
        Value rightValue = eval_expression(binary->right.get(), environment);

        if (binary->op == "+") {
            if ((std::holds_alternative<int>(leftValue) || std::holds_alternative<double>(leftValue) || std::holds_alternative<bool>(leftValue)) &&
                (std::holds_alternative<int>(rightValue) || std::holds_alternative<double>(rightValue) || std::holds_alternative<bool>(rightValue))) {
                double left = value_to_number(leftValue);
                double right = value_to_number(rightValue);
                if (std::holds_alternative<int>(leftValue) && std::holds_alternative<int>(rightValue)) {
                    return static_cast<int>(left + right);
                }
                return left + right;
            }
            return value_to_string(leftValue) + " " + value_to_string(rightValue);
        }
        if (binary->op == "-") {
            double left = value_to_number(leftValue);
            double right = value_to_number(rightValue);
            if (std::holds_alternative<int>(leftValue) && std::holds_alternative<int>(rightValue)) {
                return static_cast<int>(left - right);
            }
            return left - right;
        }
        if (binary->op == "*") {
            double left = value_to_number(leftValue);
            double right = value_to_number(rightValue);
            if (std::holds_alternative<int>(leftValue) && std::holds_alternative<int>(rightValue)) {
                return static_cast<int>(left * right);
            }
            return left * right;
        }
        if (binary->op == "/") {
            double left = value_to_number(leftValue);
            double right = value_to_number(rightValue);
            return left / right;
        }
        if (binary->op == "%") {
            double left = value_to_number(leftValue);
            double right = value_to_number(rightValue);
            return static_cast<int>(left) % static_cast<int>(right);
        }
        if (binary->op == "&&") {
            return is_truthy(leftValue) && is_truthy(rightValue);
        }
        if (binary->op == "||") {
            return is_truthy(leftValue) || is_truthy(rightValue);
        }

        double left = value_to_number(leftValue);
        double right = value_to_number(rightValue);

        if (binary->op == "==") {
            return left == right;
        }
        if (binary->op == "!=") {
            return left != right;
        }
        if (binary->op == "<") {
            return left < right;
        }
        if (binary->op == ">") {
            return left > right;
        }
        if (binary->op == "<=") {
            return left <= right;
        }
        if (binary->op == ">=") {
            return left >= right;
        }
    }

    throw std::runtime_error("Unknown expression");
}

bool Interpreter::interpret_nodes(const std::vector<std::unique_ptr<Statement>>& nodes,
    const std::shared_ptr<Environment>& environment) {
    for (const auto& node : nodes) {
        if (dynamic_cast<const BreakStatement*>(node.get())) {
            return true;
        }
        if (const auto* declaration = dynamic_cast<const VarDeclaration*>(node.get())) {
            static const std::unordered_set<std::string> keywords = {
                "let","output","request","if","else","elseif","func","define",
                "while","break","foreach","const","assert"
            };
            if (keywords.find(declaration->name) != keywords.end()) {
                throw std::runtime_error("Cannot assign to keyword: " + declaration->name);
            }
            Value value = eval_expression(declaration->value.get(), environment);
            environment->define(declaration->name, value, declaration->isMutable);
            continue;
        }
        if (const auto* reassignment = dynamic_cast<const VarReassignment*>(node.get())) {
            if (reassignment->op.has_value()) {
                Value value = eval_expression(reassignment->value.get(), environment);
                Value current = environment->get(reassignment->name);
                const std::string op = reassignment->op.value();

                if (op == "+=") {
                    if ((std::holds_alternative<int>(current) || std::holds_alternative<double>(current) || std::holds_alternative<bool>(current)) &&
                        (std::holds_alternative<int>(value) || std::holds_alternative<double>(value) || std::holds_alternative<bool>(value))) {
                        double left = value_to_number(current);
                        double right = value_to_number(value);
                        if (std::holds_alternative<int>(current) && std::holds_alternative<int>(value)) {
                            environment->assign(reassignment->name, static_cast<int>(left + right));
                        } else {
                            environment->assign(reassignment->name, left + right);
                        }
                    } else {
                        environment->assign(reassignment->name, value_to_string(current) + " " + value_to_string(value));
                    }
                } else if (op == "-=") {
                    double left = value_to_number(current);
                    double right = value_to_number(value);
                    if (std::holds_alternative<int>(current) && std::holds_alternative<int>(value)) {
                        environment->assign(reassignment->name, static_cast<int>(left - right));
                    } else {
                        environment->assign(reassignment->name, left - right);
                    }
                } else if (op == "*=") {
                    double left = value_to_number(current);
                    double right = value_to_number(value);
                    if (std::holds_alternative<int>(current) && std::holds_alternative<int>(value)) {
                        environment->assign(reassignment->name, static_cast<int>(left * right));
                    } else {
                        environment->assign(reassignment->name, left * right);
                    }
                } else if (op == "/=") {
                    double left = value_to_number(current);
                    double right = value_to_number(value);
                    environment->assign(reassignment->name, left / right);
                } else if (op == "%=") {
                    double left = value_to_number(current);
                    double right = value_to_number(value);
                    environment->assign(reassignment->name, static_cast<int>(left) % static_cast<int>(right));
                } else {
                    throw std::runtime_error("Unsupported assignment operator");
                }
            } else {
                environment->assign(reassignment->name, eval_expression(reassignment->value.get(), environment));
            }
            continue;
        }
        if (const auto* output = dynamic_cast<const OutputStatement*>(node.get())) {
            Value value = eval_expression(output->value.get(), environment);
            std::cout << value_to_string(value) << std::endl;
            continue;
        }
        if (const auto* request = dynamic_cast<const RequestStatement*>(node.get())) {
            eval_expression(request, environment);
            continue;
        }
        if (const auto* ifStmt = dynamic_cast<const IfStatement*>(node.get())) {
            if (is_truthy(eval_expression(ifStmt->condition.get(), environment))) {
                auto block_environment = std::make_shared<Environment>(environment);
                if (interpret_nodes(ifStmt->body, block_environment)) {
                    return true;
                }
            } else if (!ifStmt->elseBody.empty()) {
                if (ifStmt->elseBody.size() == 1 && dynamic_cast<const IfStatement*>(ifStmt->elseBody.front().get())) {
                    if (interpret_nodes(ifStmt->elseBody, environment)) {
                        return true;
                    }
                } else {
                    auto block_environment = std::make_shared<Environment>(environment);
                    if (interpret_nodes(ifStmt->elseBody, block_environment)) {
                        return true;
                    }
                }
            }
            continue;
        }
        if (const auto* function = dynamic_cast<const Function*>(node.get())) {
            functions[function->name] = function;
            continue;
        }
        if (const auto* call = dynamic_cast<const FunctionCall*>(node.get())) {
            auto it = functions.find(call->name);
            if (it == functions.end()) {
                throw std::runtime_error("Not callable: " + call->name);
            }
            auto function_environment = std::make_shared<Environment>(environment);
            if (interpret_nodes(it->second->body, function_environment)) {
                return true;
            }
            continue;
        }
        if (const auto* whileLoop = dynamic_cast<const WhileLoop*>(node.get())) {
            while (is_truthy(eval_expression(whileLoop->condition.get(), environment))) {
                auto block_environment = std::make_shared<Environment>(environment);
                if (interpret_nodes(whileLoop->body, block_environment)) {
                    break;
                }
            }
            continue;
        }
        if (const auto* foreachLoop = dynamic_cast<const ForeachLoop*>(node.get())) {
            Value collection = eval_expression(foreachLoop->collection.get(), environment);
            if (std::holds_alternative<std::string>(collection)) {
                const std::string& text = std::get<std::string>(collection);
                for (const char c : text) {
                    auto block_environment = std::make_shared<Environment>(environment);
                    block_environment->define(foreachLoop->name, std::string(1, c), true);
                    if (interpret_nodes(foreachLoop->body, block_environment)) {
                        break;
                    }
                }
            } else {
                throw std::runtime_error("Foreach collection is not iterable");
            }
            continue;
        }
        if (const auto* assertStmt = dynamic_cast<const AssertStatement*>(node.get())) {
            if (!is_truthy(eval_expression(assertStmt->condition.get(), environment))) {
                if (assertStmt->message.has_value()) {
                    throw std::runtime_error(assertStmt->message.value());
                }
                throw std::runtime_error("Assertion failed");
            }
            continue;
        }
        throw std::runtime_error("Unknown statement");
    }
    return false;
}

bool Interpreter::is_truthy(const Value& value) const {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    }
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    }
    if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    }
    return false;
}

std::string Interpreter::value_to_string(const Value& value) const {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    }
    if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    }
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    }
    return "";
}

double Interpreter::value_to_number(const Value& value) {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    }
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    if (std::holds_alternative<std::string>(value)) {
        const std::string& text = std::get<std::string>(value);
        size_t idx = 0;
        try {
            double parsed = std::stod(text, &idx);
            if (idx == text.size()) {
                return parsed;
            }
        } catch (const std::exception&) {
        }
        throw std::runtime_error("Invalid numeric value: " + text);
    }
    throw std::runtime_error("Invalid numeric value");
}
