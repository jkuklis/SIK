#include <iostream>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "siktacka-input-server.h"


void send_string(pollfd &sock, std::string &str, sockaddr_in6 &addr) {
    int flags = 0;
    socklen_t snda_len = (socklen_t) sizeof(addr);

    int len = str.size();

    int sndlen = sendto(sock.fd, str.c_str(), (size_t) len, flags,
            (struct sockaddr *) &addr, snda_len);

    assert(len == sndlen);
}


bool fill_server_params(server_params &sp, int argc, char *argv[]) {
    char c;
    uint32_t to_int;
    std::string arg;
    constraints<uint32_t> constr;

    if (argc != 1 && argv[1][0] != '-')
        return false;

    // Width Height port speed turn random
    while ((c = getopt(argc, argv, "W:H:p:s:t:r:")) != -1) {
        arg = optarg;

        bool successful_conversion = string_to_int(arg, to_int);
        if (!successful_conversion)
            return false;

        switch (c) {
            case 'W':
                constr = constraints<uint32_t>("width", MIN_WIDTH, MAX_WIDTH);
                sp.width = to_int;
                break;
            case 'H':
                constr = constraints<uint32_t>("height", MIN_HEIGHT, MAX_HEIGHT);
                sp.height = to_int;
                break;
            case 'p':
                constr = constraints<uint32_t>("port", MIN_PORT, MAX_PORT);
                sp.port = to_int;
                break;
            case 's':
                constr = constraints<uint32_t>("speed", MIN_SPEED, MAX_SPEED);
                sp.speed = to_int;
                break;
            case 't':
                constr = constraints<uint32_t>("turn", MIN_TURN, MAX_TURN);
                sp.turn = to_int;
                break;
            case 'r':
                constr = constraints<uint32_t>("random", 0, MAX_UINT32);
                sp.random = to_int;
                break;
            case '?':
                std::cout << "Unknown option\n";
                return false;
            default:
                std::cout << "Error during parsing command line\n";
                return false;
        }

        if (!checked_constraints<uint32_t>(to_int, constr))
            return false;
    }
    return true;
}
