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

    cp.player_name = argv[1];

    if (argc == 4 && !check_host(argv[3], cp, !server))
        return false;

    return true;
}
