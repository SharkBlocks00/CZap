#ifndef CZAP_IDENTIFIER_H
#define CZAP_IDENTIFIER_H
#include <memory>
#include <string>

#include "Expression.h"


class Identifier : public Expression {
public:
    explicit Identifier(std::string value);

    std::string value;
};


#endif //CZAP_IDENTIFIER_H
