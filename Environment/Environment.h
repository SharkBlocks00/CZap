#ifndef CZAP_ENVIRONMENT_H
#define CZAP_ENVIRONMENT_H
#include <memory>
#include <unordered_map>

#include "../Helpers/Value.h"


class Environment {
public:
    explicit Environment(std::shared_ptr<Environment> parent = nullptr);

    void define(std::string name, Value value, bool isMutable);

    void assign(std::string name, Value value);

    Value get(std::string name);

private:

    std::shared_ptr<Environment> parent;
    std::unordered_map<std::string, std::tuple<Value, bool>> variables; // name -> (value, isMutable)

};


#endif //CZAP_ENVIRONMENT_H