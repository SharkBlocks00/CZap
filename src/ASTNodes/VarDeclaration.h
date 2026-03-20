
#ifndef CZAP_VARDECLARATION_H
#define CZAP_VARDECLARATION_H
#include <memory>
#include <string>

#include "Expression.h"
#include "Statement.h"


class VarDeclaration : public Statement {
public:
    VarDeclaration(const std::string& name, std::unique_ptr<Expression> value, bool isMutable = true);

    std::string name;
    std::unique_ptr<Expression> value;
    bool isMutable;


};


#endif //CZAP_VARDECLARATION_H