//
// Created by ogkbe on 3/18/2026.
//

#ifndef CZAP_EXPRESSION_H
#define CZAP_EXPRESSION_H
#include "ASTNode.h"


class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};


#endif //CZAP_EXPRESSION_H