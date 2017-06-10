#include <iostream>

#include "siktacka-input.h"
#include "siktacka-consts.h"

bool is_numeric(std::string &str) {

    if (str.empty())
        return false;

    return std::all_of(str.begin(), str.end(),
                [](unsigned char c) { return ::isdigit(c); });
}


// TODO update uint32_t

bool string_to_int(std::string &str, uint32_t &result) {
    int64_t tmp;

    if (!is_numeric(str)) {
        std::cout << "Non numeric argument\n";
        return false;
    }

    if (str.length() > 9) {
        std::cout << "Out of range argument\n";
        return false;
    }

    try {
        tmp = std::strtol(str.c_str(), NULL, 10);

        if (tmp < 0 || tmp > MAX_UINT32) {
            std::cout << "Out of range argument\n";
            return false;
        }

        result = (uint32_t) tmp;

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


bool check_player_name(std::string &str, bool print) {
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

std::vector<std::string> split_to_vector(std::string &str, char delimiter_char,
            uint32_t len) {

    std::string delimiter;
    std::string to_push;
    std::vector<std::string> parts;
    size_t last = 0;
    size_t next = 0;

    if (delimiter_char != '\0') {

        delimiter = "";
        delimiter += delimiter_char;

        while ((next = str.find(delimiter, last)) != std::string::npos) {
            to_push = str.substr(last, next - last);

            parts.push_back(to_push);

            last = next + 1;
        }
        parts.push_back(str.substr(last));

    } else {

        for (uint32_t i = 0; i < len; i++) {
            if (str[i] == '\0') {
                to_push = str.substr(last, i - last);

                parts.push_back(to_push);

                last = i + 1;
            }
        }

        parts.push_back(str.substr(last));
    }

    return parts;
}
