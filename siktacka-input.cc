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

bool checked_constraints(int value, constraints_info constr) {
    if (value < constr.min_value) {
        std::cout << constr.param_name << " too small\n";
        return false;
    }
    if (value > constr.max_value) {
        std::cout << constr.param_name << " too big\n";
        return false;
    }
    return true;
}
