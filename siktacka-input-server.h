#ifndef SIKTACKA_INPUT_SERVER
#define SIKTACKA_INPUT_SERVER

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <assert.h>

#include "siktacka-consts.h"
#include "siktacka-event-struct.h"

void send_event(pollfd &sock, event &ev, sockaddr_in6 &addr) {
    int flags = 0;
    socklen_t snda_len = (socklen_t) sizeof(addr);

    std::string ev_str = event_str(ev);

    int len = ev_str.size();

    int sndlen = sendto(sock.fd, ev_str.c_str(), (size_t) len, flags,
            (struct sockaddr *) &addr, snda_len);

    assert(len == sndlen);
}


struct broadcaster {

    sockaddr_in6 addr[MAX_PLAYERS];
    bool connected[MAX_PLAYERS];
    pollfd sock;

    void broadcast(event &ev) {
        for (uint32_t i = 0; i < MAX_PLAYERS; i++)
            if (connected[i])
                send_event(sock, ev, addr[i]);
    };
};

struct server_params {
    uint32_t width = DEFAULT_WIDTH;
    uint32_t height = DEFAULT_HEIGHT;
    uint32_t port = DEFAULT_PORT;
    uint32_t speed = DEFAULT_SPEED; // ROUND_PER_SEC
    uint32_t turn = DEFAULT_TURN; // TURNING_SPEED
    uint32_t random = time(NULL); // rand() seed
    broadcaster broad;
};

void print_server_params(server_params sp);

bool fill_server_params(server_params &sp, int argc, char *argv[]);

#endif /* SIKTACKA_INPUT_SERVER */
