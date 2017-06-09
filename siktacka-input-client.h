#ifndef SIKTACKA_INPUT_CLIENT
#define SIKTACKA_INPUT_CLIENT

#include <string>
#include "siktacka-consts.h"

struct client_params {
    std::string player_name;
    std::string server_host;
    int server_port = DEFAULT_PORT;
    std::string ui_host = DEFAULT_HOST;
    int ui_port = DEFAULT_GUI_PORT;
};

void print_client_params(client_params cp);

bool fill_client_params(client_params &cp, int argc, char *argv[]);

#endif /* SIKTACKA_INPUT_CLIENT */
