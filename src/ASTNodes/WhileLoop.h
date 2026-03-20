#pragma once

#include "Statement.h"
#include "Expression.h"
#include <memory>
#include <vector>

class WhileLoop : public Statement {
public:
    explicit WhileLoop(std::unique_ptr<Expression> condition,
                       std::vector<std::unique_ptr<Statement>> body);

    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
};