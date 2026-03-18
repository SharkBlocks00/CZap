//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_VARREASSIGNMENT_H
#define CZAP_VARREASSIGNMENT_H
#include <memory>
#include <optional>
#include <string>

#include "Expression.h"
#include "Statement.h"

class Expression;
class Statement;

class VarReassignment : public Statement {
public:
    VarReassignment(const std::string&   name,
        std::unique_ptr<Expression> value,
        std::optional<std::string> op = std::nullopt);

    std::string name;
    std::unique_ptr<Expression> value;
    std::optional<std::string> op;
};


#endif //CZAP_VARREASSIGNMENT_H