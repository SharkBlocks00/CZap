//
// Created by ogkbe on 3/18/2026.
//

#include "Identifier.h"

Identifier::Identifier(std::unique_ptr<Expression> value)
    : value(std::move(value)) {}