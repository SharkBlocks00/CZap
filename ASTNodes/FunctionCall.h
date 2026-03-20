
#ifndef CZAP_FUNCTIONCALL_H
#define CZAP_FUNCTIONCALL_H
#include <memory>
#include <vector>

#include "Expression.h"
#include "Statement.h"


class FunctionCall : public Expression, public Statement {
public:
    explicit FunctionCall(std::string name,
                          std::vector<std::unique_ptr<Expression>> arguments = {});

    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;
};


#endif //CZAP_FUNCTIONCALL_H