#ifndef SIKTACKA_INPUT_SERVER
#define SIKTACKA_INPUT_SERVER

#include "siktacka-consts.h"

// TODO correct to uint32_t or sth

struct server_params {
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    int port = DEFAULT_PORT;
    int speed = DEFAULT_SPEED; // ROUND_PER_SEC
    int turn = DEFAULT_TURN; // TURNING_SPEED
    int random = time(NULL); // rand() seed
    //int game_id = 0;
};

void print_server_params(server_params sp);

bool fill_server_params(server_params &sp, int argc, char *argv[]);

#endif /* SIKTACKA_INPUT_SERVER */
