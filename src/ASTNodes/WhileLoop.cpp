#include "WhileLoop.h"

#include <memory>
#include <vector>

WhileLoop::WhileLoop(std::unique_ptr<Expression> condition,
                     std::vector<std::unique_ptr<Statement>> body)
                     : condition (std::move(condition)), body(std::move(body)) {}