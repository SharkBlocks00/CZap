//
// Created by ogkbe on 3/18/2026.
//

#include "RequestStatement.h"

RequestStatement::RequestStatement(std::unique_ptr<Expression> value)
    : value(std::move(value)) {}