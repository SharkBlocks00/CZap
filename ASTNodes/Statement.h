//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_STATEMENT_H
#define CZAP_STATEMENT_H
#include "ASTNode.h"


class Statement : public ASTNode {
    public:
    virtual ~Statement() = default;
};


#endif //CZAP_STATEMENT_H