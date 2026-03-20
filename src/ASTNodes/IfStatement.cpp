#include "IfStatement.h"

IfStatement::IfStatement(std::unique_ptr<Expression> condition,
                         std::vector<std::unique_ptr<Statement>> body,
                         std::vector<std::unique_ptr<Statement>> elseBody)
        : condition (std::move(condition)), body(std::move(body)), elseBody(std::move(elseBody)) {}