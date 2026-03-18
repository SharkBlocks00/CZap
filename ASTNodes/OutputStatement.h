//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_OUTPUTSTATEMENT_H
#define CZAP_OUTPUTSTATEMENT_H
#include <memory>

#include "Expression.h"
#include "Statement.h"


class OutputStatement : public Statement {
public:
    explicit OutputStatement(std::unique_ptr<Expression> value);

    std::unique_ptr<Expression> value;
};


#endif //CZAP_OUTPUTSTATEMENT_H