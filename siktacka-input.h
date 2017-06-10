#ifndef SIKTACKA_INPUT
#define SIKTACKA_INPUT

#include <algorithm>
#include <string>
#include <vector>
#include <string.h>

template <typename T>
struct constraints {
    std::string param_name;
    T min_value;
    T max_value;
    constraints(std::string param_name, T min_value, T max_value) :
            param_name(param_name),
            min_value(min_value),
            max_value(max_value) {}
    constraints() {}
};


bool is_numeric(std::string &str);


bool string_to_int(std::string &str, uint32_t &result);


template <typename T>
bool checked_constraints(T value, constraints<T> constr, bool print = true) {
    if (value < constr.min_value) {

        if (print) {
            std::cout << constr.param_name << " too small\n";
        }
        return false;
    }
    if (value > constr.max_value) {
        if (print) {
            std::cout << constr.param_name << " too big\n";
        }
        return false;
    }
    return true;
}


bool check_player_name(std::string &str, bool print = true);


std::vector<std::string> split_to_vector(std::string &str, char delimiter_char,
            uint32_t len = 0);


template <typename T>
void append_data(T data, std::string &str,
            bool big_endian = true, uint32_t bytes = 0) {

    if (bytes == 0) {
        bytes = sizeof(T);
    }

    uint8_t split[bytes];

    memcpy(split, &(data), bytes);

    for (uint32_t i = 0; i < bytes; i++) {

        if (big_endian) {
            str += split[bytes - 1 - i];

        } else {
            str += split[i];
        }
    }
}


template <typename T>
T parse_number(std::string &str, uint32_t begin = 0,
            uint32_t end = (uint32_t)(-1), bool big_endian = true) {

    uint32_t index;

    if (end == (uint32_t)(-1)) {
        end = str.length();
    }

    T result = 0;

    for (uint32_t i = 0; i < end - begin; i++) {

        index = (big_endian ? (begin + i) : (end - 1 - i));

        result <<= 8;
        result += (uint8_t) str[index];
    }

    return result;
}


template <typename T>
bool append_value(T value, std::string &str) {
    std::string value_str;

    value_str = std::to_string(value);

    str += value_str;

    str += ' ';

    return true;
}

#endif /* SIKTACKA_INPUT */
