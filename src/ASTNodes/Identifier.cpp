
#include "Identifier.h"

#include <utility>

Identifier::Identifier(std::string value)
    : value(std::move(value)) {}
