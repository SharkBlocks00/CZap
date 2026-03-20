#include "AssertStatement.h"

#include <string>
#include <memory>
#include <optional>

AssertStatement::AssertStatement(std::unique_ptr<Expression> condition, std::optional<std::string> message) 
    : condition(std::move(condition)), message(std::move(message)) {}