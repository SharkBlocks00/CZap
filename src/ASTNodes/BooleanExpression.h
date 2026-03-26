
#ifndef CZAP_BOOLEANEXPRESSION_H
#define CZAP_BOOLEANEXPRESSION_H
#include <memory>
#include <string>

#include "Expression.h"


class BooleanExpression : public Expression {
public:
    BooleanExpression(std::unique_ptr<Expression> left, std::string op, std::unique_ptr<Expression> right);

    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;
};


#endif //CZAP_BOOLEANEXPRESSION_H