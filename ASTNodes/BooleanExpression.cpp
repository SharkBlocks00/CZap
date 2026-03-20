
#include "BooleanExpression.h"

BooleanExpression::BooleanExpression(std::unique_ptr<Expression> left,
    std::string op,
    std::unique_ptr<Expression> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}