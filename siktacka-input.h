#ifndef SIKTACKA_INPUT
#define SIKTACKA_INPUT

#include <algorithm>
#include <string>
#include <vector>

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

bool checked_constraints(int value, constraints_info constr, bool print);

bool check_player_name(std::string str);

std::vector<std::string> split_to_vector(std::string &str, std::string delimiter);

// specify false reversed, if parsing network-formatted data
// specify length for strings which may contain '\0'
template <typename T>
void append_data(T data, std::string &str,
            bool reversed = true, uint32_t bytes = 0) {

    if (bytes == 0) {
        bytes = sizeof(T);
    }

    uint8_t split[bytes];

    memcpy(split, &(data), bytes);

    for (int i = 0; i < bytes; i++) {

        if (reversed) {
            str += split[bytes - 1 - i];

        } else {
            str += split[i];
        }
    }
}


template <typename T>
T parse_number(std::string &str, uint32_t begin = 0, uint32_t end = -1) {

    if (end == -1) {
        end = str.length();
    }

    T result = 0;

    for (uint32_t i = end - 1; i >= begin; i--) {

        result += (uint8_t) str[i];
        result <<= 8;
    }

    return result;
}


template <typename T>
bool append_value(T value, std::string &str) {
    std::string value_str;

    value_str = to_string(value);

    str += value_str;

    str += ' ';

    return true;
}

#endif /* SIKTACKA_INPUT */
