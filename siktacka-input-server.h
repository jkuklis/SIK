#ifndef SIKTACKA_INPUT_SERVER
#define SIKTACKA_INPUT_SERVER

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <vector>

#include "siktacka-consts.h"
#include "siktacka-input.h"

// send str to address via a socket
void send_string(pollfd &sock, std::string &str, sockaddr_in6 &addr);


struct server_params {
    uint32_t width = DEFAULT_WIDTH;
    uint32_t height = DEFAULT_HEIGHT;
    uint32_t port = DEFAULT_PORT;
    uint32_t speed = DEFAULT_SPEED; // ROUND_PER_SEC
    uint32_t turn = DEFAULT_TURN; // TURNING_SPEED
    uint32_t random = time(NULL); // rand() seed
};


// fill and check correctness of server parameters
bool fill_server_params(server_params &sp, int argc, char *argv[]);


#endif /* SIKTACKA_INPUT_SERVER */
