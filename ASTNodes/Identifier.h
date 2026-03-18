//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_IDENTIFIER_H
#define CZAP_IDENTIFIER_H
#include <memory>

#include "Expression.h"


class Identifier : Expression {
public:
    explicit Identifier(std::unique_ptr<Expression> value);

    std::unique_ptr<Expression> value;
};


#endif //CZAP_IDENTIFIER_H