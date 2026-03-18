//
// Created by ogkbe on 3/18/2026.
//

#include "VarDeclaration.h"

VarDeclaration::VarDeclaration(const std::string& name,
    std::unique_ptr<Expression> value,
    const bool isMutable)
        : name(name), value(std::move(value)), isMutable(isMutable) {}
