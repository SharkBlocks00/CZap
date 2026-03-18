//
// Created by ogkbe on 3/18/2026.
//

#include "BinaryExpression.h"

BinaryExpression::BinaryExpression(std::unique_ptr<Expression> left,
                                   std::string str,
                                   std::unique_ptr<Expression> right)
    : left(std::move(left)),
      right(std::move(right)),
      str(std::move(str)) {}