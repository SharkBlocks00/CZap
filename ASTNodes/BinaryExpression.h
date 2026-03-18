//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_BINARYEXPRESSION_H
#define CZAP_BINARYEXPRESSION_H
#include <memory>

#include "Expression.h"


class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> left,
        std::string str,
        std::unique_ptr<Expression> right);

    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string str;
};


#endif //CZAP_BINARYEXPRESSION_H