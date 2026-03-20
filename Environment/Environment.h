#ifndef CZAP_ENVIRONMENT_H
#define CZAP_ENVIRONMENT_H
#include <optional>
#include <memory>

#include "../Helpers/Value.h"


class Environment {
public:
    explicit Environment(std::shared_ptr<Environment> parent = nullptr);

    void define(std::string name, Value value, bool isMutable);

    void assign(std::string name, Value value);

    Value get(std::string name);

private:

    std::shared_ptr<Environment> parent;

};


#endif //CZAP_ENVIRONMENT_H