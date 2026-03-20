#pragma once

#include "Statement.h"
#include "Expression.h"
#include <optional>
#include <memory>
#include <string>

class AssertStatement : public Statement {
public:
    explicit AssertStatement(std::unique_ptr<Expression> condition, std::optional<std::string> message = std::nullopt);

    std::unique_ptr<Expression> condition;
    std::optional<std::string> message;
};