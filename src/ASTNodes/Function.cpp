#include "Function.h"

#include <string>
#include <vector>
#include <memory>

Function::Function(std::string name, std::vector<std::string> parameters, std::vector<std::unique_ptr<Statement>> body) 
    : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}