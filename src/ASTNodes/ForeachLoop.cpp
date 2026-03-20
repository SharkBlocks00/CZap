#include "ForeachLoop.h"

ForeachLoop::ForeachLoop(std::string name, std::unique_ptr<Expression> collection, std::vector<std::unique_ptr<Statement>> body) 
    : name(std::move(name)), collection(std::move(collection)), body(std::move(body)) {}