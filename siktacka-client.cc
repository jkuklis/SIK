#include <iostream>
#include <string>

#include <sys/time.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "siktacka-input-client.h"
#include "siktacka-establish-client.h"
#include "siktacka-communication-server.h"

const int USLEEP_TIME = 20;

int check_ui() {
    return 0;
}

int main(int argc, char *argv[]) {

    client_params cp;
    int server_sock;
    int ui_sock;
    sockaddr_in6 server_address;
    sockaddr_in6 ui_address;

    if (!fill_client_params(cp, argc, argv))
        return 1;

    print_client_params(cp);

    if (!establish_address(server_address, cp.server_host, cp.server_port))
        return 1;

    if (!get_socket(server_sock))
        return 1;

    if (!establish_address(ui_address, cp.ui_host, cp.ui_port))
        return 1;

    if (!get_socket(ui_sock))
        return 1;

    timeval start;
    gettimeofday(&start, NULL);
    uint64_t now = 1000 * start.tv_sec + start.tv_usec;
    uint64_t last_sent = now;

    message_cts msg_cts(last_sent, 0, 0, cp.player_name);

    std::string msg_cts_str = message_cts_str(msg_cts);
    size_t len = msg_cts_str.length();
    int flags = 0;
    socklen_t addrlen = (socklen_t) sizeof(server_address);

    ssize_t send_len;

    while(true) {
        send_len = sendto(server_sock, msg_cts_str.c_str(), len, flags,
                (struct sockaddr *) &server_address, addrlen);

        msg_cts.turn_direction = check_ui();

        gettimeofday(&start, NULL);
        now = 1000 * start.tv_sec + start.tv_usec;
        //int64_t sleep_time = USLEEP_TIME - (now - last_sent);
        //last_sent = now;

        //if (sleep_time >= 0)
        //    usleep(sleep_time);

        //std::cout << now << "  ";

    }


    return 0;
}
