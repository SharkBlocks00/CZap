
#include "OutputStatement.h"

OutputStatement::OutputStatement(std::unique_ptr<Expression> value)
    : value(std::move(value)) {}