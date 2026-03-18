//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_BOOLEANLITERAL_H
#define CZAP_BOOLEANLITERAL_H
#include <string>

#include "Expression.h"


class BooleanLiteral : Expression {
public:
    explicit BooleanLiteral(bool value);

    bool value;
};


#endif //CZAP_BOOLEANLITERAL_H