//
// Created by ogkbe on 3/18/2026.
//

#include "OutputStatement.h"

OutputStatement::OutputStatement(std::unique_ptr<Expression> value)
    : value(std::move(value)) {}