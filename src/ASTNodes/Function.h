#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Statement.h"

class Function : public Statement {
public:
    explicit Function(std::string name, std::vector<std::string> parameters, std::vector<std::unique_ptr<Statement>> body);\

    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<Statement>> body;
};