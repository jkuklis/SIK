#include <iostream>
#include <string>
#include <unistd.h>
#include <getopt.h>

#include "siktacka-consts.h"
#include "siktacka-input.h"
#include "siktacka-input-server.h"

void print_server_params(server_params sp) {
    std::cout << sp.width << " " << sp.height << " " << sp.port << " " \
                << sp.speed << " " << sp.turn << " " << sp.random << std::endl;
}

bool fill_server_params(server_params &sp, int argc, char *argv[]) {
    char c;
    int to_int;
    constraints_info constr;

    // Width Height port speed turn random
    while ((c = getopt(argc, argv, "W:H:p:s:t:r:")) != -1) {
        bool successful_conversion = string_to_int(optarg, to_int);
        if (!successful_conversion)
            return false;

        switch (c) {
            case 'W':
                constr = constraints_info("width", MIN_WIDTH, MAX_WIDTH);
                sp.width = to_int;
                break;
            case 'H':
                constr = constraints_info("height", MIN_HEIGHT, MAX_HEIGHT);
                sp.height = to_int;
                break;
            case 'p':
                constr = constraints_info("port", MIN_PORT, MAX_PORT);
                sp.port = to_int;
                break;
            case 's':
                constr = constraints_info("speed", MIN_SPEED, MAX_SPEED);
                sp.speed = to_int;
                break;
            case 't':
                constr = constraints_info("turn", MIN_TURN, MAX_TURN);
                sp.turn = to_int;
                break;
            case 'r':
                constr = constraints_info("random", MIN_INT, MAX_INT);
                sp.random = to_int;
                break;
            case '?':
                std::cout << "Unknown option\n";
                return false;
            default:
                std::cout << "Error during parsing command line\n";
                return false;
        }

        if (!checked_constraints(to_int, constr))
            return false;
    }
    return true;
}
