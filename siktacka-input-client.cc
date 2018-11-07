#include <iostream>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <vector>

#include "siktacka-consts.h"
#include "siktacka-input.h"
#include "siktacka-input-client.h"


bool get_port(std::string &port_str, client_params &cp, bool server) {
    uint32_t port;
    constraints<uint32_t> constr;

    if (!string_to_int(port_str, port))
        return false;


    std::string helper = (server ? "server" : "gui");

    constr = constraints<uint32_t>("port of " + helper, MIN_PORT, MAX_PORT);


    if (!checked_constraints<uint32_t>(port, constr))
        return false;


    if (server)
        cp.server_port = port;
    else
        cp.ui_port = port;

    return true;
}


bool check_host(std::string &str, client_params &cp, bool server) {

    std::vector<std::string> parts = split_to_vector(str, ':');

    // possible port number involved
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

    std::string name;
    std::string server_host;
    std::string gui_host;

    if (argc != 3 && argc != 4) {
        std::cout << "Usage: ./siktacka-client player_name " <<
                "game_server_host[:port] [ui_server_host[:port]]\n";
        return false;
    }

    bool server = true;

    name = argv[1];
    server_host = argv[2];

    if (!check_player_name(name) || !check_host(server_host, cp, server))
        return false;

    cp.player_name = argv[1];


    if (argc == 4) {
        gui_host = argv[3];

        if (!check_host(gui_host, cp, !server))
            return false;
    }

    return true;
}
