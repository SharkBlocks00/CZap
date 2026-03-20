
#ifndef CZAP_NUMBERLITERAL_H
#define CZAP_NUMBERLITERAL_H

#include "Expression.h"


class NumberLiteral : public Expression {
public:
     explicit NumberLiteral(int value);

     int value;
};


#endif //CZAP_NUMBERLITERAL_H
