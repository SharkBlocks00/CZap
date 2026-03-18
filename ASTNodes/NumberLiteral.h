//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_NUMBERLITERAL_H
#define CZAP_NUMBERLITERAL_H
#include <memory>

#include "Expression.h"


class NumberLiteral : Expression {
public:
     explicit NumberLiteral(int value);

     int value;
};


#endif //CZAP_NUMBERLITERAL_H