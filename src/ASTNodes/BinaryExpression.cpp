
#include "BinaryExpression.h"

BinaryExpression::BinaryExpression(std::unique_ptr<Expression> left,
                                   std::string op,
                                   std::unique_ptr<Expression> right)
    : left(std::move(left)),
      right(std::move(right)),
      op(std::move(op)) {}