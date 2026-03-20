#ifndef CZAP_VALUE_H
#define CZAP_VALUE_H
#include <string>
#include <variant>


using Value = std::variant<
    int,
    double,
    std::string,
    bool
>;


#endif //CZAP_VALUE_H