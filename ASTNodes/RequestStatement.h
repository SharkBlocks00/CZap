//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_REQUESTSTATEMENT_H
#define CZAP_REQUESTSTATEMENT_H
#include <memory>

#include "Expression.h"
#include "Statement.h"


class RequestStatement : public Expression, public Statement {
public:
    explicit RequestStatement(std::unique_ptr<Expression> value);

    std::unique_ptr<Expression> value;
};


#endif //CZAP_REQUESTSTATEMENT_H