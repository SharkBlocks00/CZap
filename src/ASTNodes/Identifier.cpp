
#include "Identifier.h"

Identifier::Identifier(std::unique_ptr<Expression> value)
    : value(std::move(value)) {}