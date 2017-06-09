#ifndef SIKTACKA_INPUT
#define SIKTACKA_INPUT

#include <algorithm>
#include <string>

struct constraints_info {
    std::string param_name;
    int min_value;
    int max_value;
    constraints_info(std::string param_name, int min_value, int max_value) :
            param_name(param_name),
            min_value(min_value),
            max_value(max_value) {}
    constraints_info() {}
};

bool is_numeric(std::string str);

bool string_to_int(std::string str, int &result);

bool checked_constraints(int value, constraints_info constr);

#endif /* SIKTACKA_INPUT */
