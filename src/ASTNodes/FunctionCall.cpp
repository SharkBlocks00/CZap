
#include "FunctionCall.h"

#include <utility>

FunctionCall::FunctionCall(std::string name,
    std::vector<std::unique_ptr<Expression>> arguments)
        : name(std::move(name)), arguments(std::move(arguments)) {}