#pragma once
#include "Statement.h"
#include "Expression.h"
#include <memory>

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;

    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
};