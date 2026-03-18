//
// Created by ogkbe on 3/18/2026.
//

#include "VarReassignment.h"

#include <utility>

#include "Expression.h"

VarReassignment::VarReassignment(const std::string&  name,
                                 std::unique_ptr<Expression> value,
                                 std::optional<std::string> op)
    : name(name), value(std::move(value)), op(std::move(op)) {}