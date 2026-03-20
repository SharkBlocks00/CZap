
#include "RequestStatement.h"

RequestStatement::RequestStatement(std::unique_ptr<Expression> value)
    : value(std::move(value)) {}