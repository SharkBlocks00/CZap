#include "Environment.h"
#include <stdexcept>
#include <format>

Environment::Environment(std::shared_ptr<Environment> parent) // defaults parent to nullptr
    : parent(std::move(parent)) {}


void Environment::define(std::string name, Value value, bool isMutable) {
    if (variables.find(name) != variables.end()) {
        throw std::runtime_error(std::format("Variable '{}' is already defined.", name));
    }
    variables[name] = std::make_tuple(std::move(value), isMutable);
}

void Environment::assign(std::string name, Value value) {
    if (variables.find(name) == variables.end()) {
        if (parent) {
            parent->assign(std::move(name), std::move(value));
            return;
        }
        throw std::runtime_error(std::format("Variable '{}' is not defined.", name));
    }
    auto& [currentValue, isMutable] = variables[name];
    if (!isMutable) {
        throw std::runtime_error(std::format("Variable '{}' is immutable and cannot be reassigned.", name));
    }
    currentValue = std::move(value);
    return;
}

Value Environment::get(std::string name) {
    if (variables.find(name) == variables.end()) {
        if (parent) {
            return parent->get(std::move(name));
        }
        throw std::runtime_error(std::format("Variable '{}' is not defined.", name));
    }
    auto&  [value, _] = variables[name];
    return value;
}