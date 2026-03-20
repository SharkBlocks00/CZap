
#ifndef CZAP_BINARYEXPRESSION_H
#define CZAP_BINARYEXPRESSION_H
#include <memory>
#include <string>

#include "Expression.h"


class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> left,
        std::string op,
        std::unique_ptr<Expression> right);

    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string op;
};


#endif //CZAP_BINARYEXPRESSION_H