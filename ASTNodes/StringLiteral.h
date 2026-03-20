
#ifndef CZAP_STRINGLITERAL_H
#define CZAP_STRINGLITERAL_H
#include <string>

#include "Expression.h"


class StringLiteral : Expression {
public:
    explicit StringLiteral(const std::string& value);

    std::string value;
};


#endif //CZAP_STRINGLITERAL_H