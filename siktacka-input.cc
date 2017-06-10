#include "iostream"

#include "siktacka-input.h"

bool is_numeric(std::string str) {
    return std::all_of(str.begin(), str.end(),
                [](unsigned char c) { return ::isdigit(c); });
}

bool string_to_int(std::string str, int &result) {
    if (!is_numeric(str)) {
        std::cout << "Non numeric argument\n";
        return false;
    }

    try {
        result = std::stoi(str);

    } catch (std::invalid_argument& e) {
        std::cout << "Conversion not possible\n";
        return false;

    } catch (std::out_of_range& e) {
        std::cout << "Out of range argument\n";
        return false;

    } catch (...) {
        std::cout << "Unknown conversion error\n";
        return false;
    }

    return true;
}

bool checked_constraints(int value, constraints_info constr, bool print = true) {
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

bool check_player_name(std::string str, bool print = true) {
    if (str.length() < MIN_LENGTH) {

        if (print) {
            std::cout << "Player name too short\n";
        }
        return false;
    }
    if (str.length() > MAX_LENGTH) {

        if (print) {
            std::cout << "Player name too long\n";
        }
        return false;
    }
    bool correct = true;

    for (char c : str) {
        if (c < 33 || 126 < c) {
            correct = false;
            if (print) {
                std::cout << "Wrong character in player name\n";
            }
            break;
        }
    }

    return correct;
}

std::vector<std::string> split_to_vector(std::string &str, std::string delimiter) {
    std::vector<std::string> parts;
    size_t last = 0;
    size_t next = 0;

    while ((next = str.find(delimiter, last)) != std::string::npos) {
        parts.push_back(str.substr(last, next - last));
        last = next + 1;
    }
    parts.push_back(str.substr(last));

    return parts;
}
