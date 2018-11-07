#ifndef SIKTACKA_INPUT_CLIENT
#define SIKTACKA_INPUT_CLIENT

#include <string>
#include "siktacka-consts.h"


struct client_params {
    std::string player_name;
    std::string server_host;
    uint32_t server_port = DEFAULT_PORT;
    std::string ui_host = DEFAULT_HOST;
    uint32_t ui_port = DEFAULT_GUI_PORT;
    uint32_t next_expected_event_no;
};

// fill and check correctness of client parameters
bool fill_client_params(client_params &cp, int argc, char *argv[]);


#endif /* SIKTACKA_INPUT_CLIENT */
