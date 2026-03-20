#pragma once

#include "Statement.h"
#include "Expression.h"
#include <vector>
#include <memory>
#include <string>

class ForeachLoop : public Statement {
public:
    explicit ForeachLoop(std::string name, std::unique_ptr<Expression> collection, std::vector<std::unique_ptr<Statement>> body);
    
    std::string name;
    std::unique_ptr<Expression> collection;
    std::vector<std::unique_ptr<Statement>> body;
};