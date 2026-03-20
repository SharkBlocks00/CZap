#pragma once

#include "Expression.h"
#include "Statement.h"
#include <memory>
#include <vector>

class IfStatement : public Statement {
public:
    explicit IfStatement(std::unique_ptr<Expression> condition,
                         std::vector<std::unique_ptr<Statement>> body,
                         std::vector<std::unique_ptr<Statement>> elseBody = {});    

    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
    std::vector<std::unique_ptr<Statement>> elseBody;
};