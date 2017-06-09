#include <iostream>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <vector>

#include "siktacka-consts.h"
#include "siktacka-input.h"
#include "siktacka-input-client.h"


void print_client_params(client_params cp) {
    std::cout << cp.player_name << " " << cp.server_host << " " << cp.server_port
        << " " << cp.ui_host << " " << cp.ui_port << std::endl;
}


bool check_player_name(std::string str, client_params &cp) {
    if (str.length() < MIN_LENGTH) {
        std::cout << "Player name too short\n";
        return false;
    }
    if (str.length() > MAX_LENGTH) {
        std::cout << "Player name too long\n";
        return false;
    }
    bool correct = true;

    for (char c : str) {
        if (c < 33 || 126 < c) {
            correct = false;
            std::cout << "Wrong character in player name\n";
            break;
        }
    }

    cp.player_name = str;
    return correct;
}

std::vector<std::string> split_to_vector(std::string str, std::string delimiter) {
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

bool get_port(std::string port_str, client_params &cp, bool server) {
    int port;
    constraints_info constr;

    if (!string_to_int(port_str, port))
        return false;

    std::string helper = (server ? "server" : "gui");

    constr = constraints_info("port of " + helper, MIN_PORT, MAX_PORT);

    if (!checked_constraints(port, constr))
        return false;

    if (server)
        cp.server_port = port;
    else
        cp.ui_port = port;

    return true;
}


bool check_host(std::string str, client_params &cp, bool server) {
    std::vector<std::string> parts = split_to_vector(str, ":");

    if (parts.size() == 2) {
        std::string port_str = parts.back();

        str = str.substr(0, str.length() - port_str.length() - 1);

        if (!get_port(port_str, cp, server))
            return false;
    }
    
    if (str[str.length() - 1] == '.') {
        std::cout << "IP address ends with additional dot\n";
        return false;
    }

    if (server)
        cp.server_host = str;
    else
        cp.ui_host = str;

    return true;
}


bool fill_client_params(client_params &cp, int argc, char *argv[]) {

    if (argc != 3 && argc != 4) {
        std::cout << "Usage: ./siktacka-client player_name " <<
                "game_server_host[:port] [ui_server_host[:port]]\n";
        return false;
    }

    bool server = true;

    if (!check_player_name(argv[1], cp) || !check_host(argv[2], cp, server))
        return false;

    if (argc == 4 && !check_host(argv[3], cp, !server))
        return false;

    return true;
}
