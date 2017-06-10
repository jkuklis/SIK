#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <netdb.h>
#include <algorithm>

#include "siktacka-input-server.h"
#include "siktacka-establish-server.h"
#include "siktacka-communication-server.h"
#include "siktacka-game-logic.h"

// TODO final file should be siktacka_server

// NOTE: pass broadcaster

struct player {
    bool connected;
    bool ready;
    sockaddr_in6 address;
    uint64_t session_id;
    uint64_t last_sent;
    std::string name;


    player() {}

    player(bool connected, sockaddr_in6 address, uint64_t session_id,
                std::string name, uint64_t last_sent, bool ready = false) :
                connected(connected),
                address(address),
                session_id(session_id),
                name(name),
                last_sent(last_sent),
                ready(ready) {}
};


struct stats {
    int ready;
    int connected;
    int active;
    int inactive;
}


bool same_addr(sockaddr_in6 a1, sockaddr_in6 a2) {
	return (a1.sin6_addr.s6_addr == a2.sin6_addr.s6_addr
                && a1.sin6_port == a2.sin6_port);
}


int check_sock(pollfd &sock, bool in_game) {

    int ret;
    int mode = (in_game ? -1 : 0);

    sock.revents = 0;

    ret = poll(&sock, 1, mode);

    if (ret <= 0) {
        std::cout << "Unexpected poll error\n";
        return -1;
    }

    if (!(sock.revents && POLLIN)) {
        return 0;
    }

    return 1;
}


int process_msg_cts(pollfd &sock, sockaddr_in6 &client_address,
                message_cts &msg_cts) {

    int len;
    int flags;
    socklen_t rcva_len;
    char client_buf[MAX_CS_DATAGRAM];
    std::string client_str;

    flags = 0;
    rcva_len = (socklen_t) sizeof(client_address);

    len = recvfrom(sock.fd, client_buf, sizeof(client_buf), flags,
                (struct sockaddr *) &client_address, &rcva_len);

    client_str = std::string(client_buf, len);

    return message_cts_from_str(client_str, msg_cts);
}

int check_existing(std::vector<player> &players, sockaddr_in6 &client_address,
            message_cts &msg_cts, stats &sta) {

    int pos = -1;

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {

        if (same_addr(client_address, players[i].address)) {
            pos = i;

            // NOTE: can player_name change?
            if (players[i].session_id < msg_cts.session_id
                    || players[i].name != msg_cts.player_name) {

                players[i].connected = false;
                return -2;
            }

            players[i].session_id = msg_cts.session_id;
            players[i].last_sent = current_us();

            if (msg_cts.direction != 0 && player[i].ready == 0) {
                sta.ready ++;
            }

            break;
        }
    }

    return pos;
}

int make_place(std::vector<player> &players, stats &sta, message_cts &msg_cts) {

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].connected
                && current_us() - players[i].last_sent > 2000000) {

            players[i].connected = false;

            sta.connected --;

            if (player[i].name != "") {
                sta.active --;

            } else {
                sta.nonactive --;
            }
        }

        if (players[i].connected && players[i].name == msg_cts.player_name) {
            return -2;
        }
    }

    return 1;
}


int seek_place(std::vector<player> &players, stats &sta,
            sockaddr_in6 &client_address, message_cts &msg_cts) {
    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {

        if (!players[i].connected) {

            players[i] = player(true, client_address, msg_cts.session_id,
                        msg_cts.player_name, current_us());

            sta.connected ++;

            if (msg_cts.player_name != "") {
                sta.active ++;

            } else {
                sta.nonactive ++;
            }

            if (msg_cts.direction != 0) {
                sta.ready ++;
            }

            return 1;
        }
    }

    return 0;
}


int receive_msg_cts(bool in_game, pollfd &sock, std::vector<player> &players,
            /*sockaddr_in6 &client_address,*/ stats &sta) {

    message_cts msg_cts;
    sockaddr_in6 client_address;

    int checked_sock = check_sock(sock, in_game);

    if (checked_sock != 1)
        return checked_sock;

    if (!process_msg_cts(sock, client_address, msg_cts))
        return 0;

    pos = check_existing(players, client_address, msg_cts, sta);

    if (pos == -1) {
        pos = make_place(players, sta);
    }

    if (pos != -2) {
        return seek_place(players, sta, client_address, msg_cts);

    } else {
        return 0;
    }

    return 0;
}


void make_players_list(std::vector<std::string> &player_names,
            std::vector<player> players) {

    int datagram_size = 4;

    player_names = std::vector<std::string>();

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {

        if (player[i].connected && player[i].name != "") {
            datagram_size += player[i].name.size();

            if (datagram_size > MAX_UDP_DATA)
                break;

            player_names.push_back(player[i].name);
        }
    }

    std::sort(player_names.begin(), player_names.end());
}


int main(int argc, char *argv[]) {

    bool success;

    message msg_cts;

    uint32_t datagram_size;

    int ret, len, pos;

    server_params sp;

    sockaddr_in6 address;
    sockaddr_in6 client_address;

    socklen_t snda_len = (socklen_t) sizeof(client_address);

    int flags = 0;

    pollfd sock;


    uint64_t last_round;

    uint64_t round_time;

    game_state gs;

    std::vector<player> players (MAX_PLAYERS);

    std::vector<uint32_t> dir_table(MAX_PLAYERS);

    std::vector<std::string> player_names;

    int connected = 0;

    int ready = 0;

    int active = 0;

    int nonactive = 0;

    if (!fill_server_params(sp, argc, argv))
        return 1;
    print_server_params(sp);

    if (!establish_address(address, sp.port))
        return 1;

    if (!get_socket(sock))
        return 1;

    if (!bind_socket(sock, address))
        return 1;


    round_time = 1000000 / sp.speed;

    in_game = false;

    while(true) {

        int res = receive_msg_cts(in_game, sock, players, sta);

        if (res == -1)
            return -1;

        if (res == 0)
            break;

        // game start
        if (sta.active >= 2 && sta.ready + sta.nonactive == sta.connected) {

            in_game = true;

            make_players_list(player_names, players);

            dir_table = std::vector<uint8_t>(MAX_PLAYERS);

            gs = new_game(player_names, sp);

            while (true) {

                while (current_us() - last_round > round_time) {
                    sock.revents = 0;

                    int res = receive_msg_cts(in_game, sock, players, sta);

                    if (res == -1)
                        return -1;
                }

                last_round += round_time;

                round(gs, sp, dir_table);

                if (gs.all_events.back().event_type == GAME_OVER)
                    break;
            }

            ready = 0;
        }
    }

    return 0;
}
