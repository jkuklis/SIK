#include <iostream>
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
#include <algorithm>
#include <assert.h>

#include "siktacka-input-server.h"
#include "siktacka-establish-server.h"
#include "siktacka-communication-server.h"
#include "siktacka-game-logic.h"


// all important information about player (connected client)
struct player {
    bool connected;
    bool ready;
    sockaddr_in6 address;
    uint64_t session_id;
    std::string name;
    uint64_t last_sent;
    int8_t direction;
    uint32_t expected_no;
    uint8_t id;

    player() {}

    player(bool connected, sockaddr_in6 &address, uint64_t session_id,
                std::string &name, uint64_t last_sent, int8_t direction,
                uint32_t expected_no) :
                connected(connected),
                address(address),
                session_id(session_id),
                name(name),
                last_sent(last_sent),
                direction(direction),
                expected_no(expected_no),
                id(MAX_PLAYERS) {

        if (direction != 0)
            ready = true;
        else
            ready = false;
    }
};


// structure to check, if enough players agreed to start game
struct stats {
    int ready;
    int connected;
    int inactive;

    stats() :
        ready(0),
        connected(0),
        inactive(0) {}
};


// comparte two addresses
bool same_address(sockaddr_in6 &a1, sockaddr_in6 &a2) {
	bool result = true;

    if (a1.sin6_port != a2.sin6_port)
        result = false;

    for (uint32_t i = 0; i < S6_ADDR_LEN; i++) {
        if (a1.sin6_addr.s6_addr[i] != a2.sin6_addr.s6_addr[i])
            result = false;
    }

    return result;
}


// check, if there is data available
// do not wait in game mode
int check_socket(pollfd &sock, bool in_game) {

    int ret;
    int mode = (in_game ? 0 : 20);

    sock.revents = 0;

    ret = poll(&sock, 1, mode);

    if (ret == -1) {
        std::cout << "Unexpected poll error\n";
        return -1;
    }

    if (ret == 0)
        return 0;

    if (!(sock.revents && POLLIN)) {
        return 0;
    }

    return 1;
}


int process_msg_cts(pollfd &sock, sockaddr_in6 &client_address,
                message_cts &msg) {

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

    return message_cts_from_str(client_str, msg);
}


bool update_stats (stats &sta, std::vector<player> &players) {
    sta = stats();

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {
        if (current_us() - players[i].last_sent > 2000000)
            players[i].connected = false;

        if (players[i].connected) {
            sta.connected ++;

            if (players[i].name == "")
                sta.inactive ++;

            else if (players[i].ready)
                sta.ready ++;
        }
    }

    return true;
}


// check, if client already connected
int check_if_in_players(std::vector<player> &players, sockaddr_in6 &address,
                        std::string &name) {

    int result = MAX_PLAYERS;

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].connected && players[i].name == name) {
            if (!same_address(players[i].address, address))
                return -1;

            result = i;
        }
    }

    return result;
}


// try find a place to attach client
uint32_t try_find_slot(std::vector<player> &players) {
    uint32_t index;

    for (index = 0; index < MAX_PLAYERS; index++) {
        if (!players[index].connected)
            break;
    }

    return index;
}


void add_player(std::vector<player> &players, int pos, message_cts &msg,
                sockaddr_in6 &address) {

    players[pos] = player(true, address, msg.session_id, msg.player_name, current_us(),
                            msg.turn_direction, msg.next_expected_event_no);

}


void update_player(std::vector<player> &players, int pos, message_cts &msg) {
    if (players[pos].session_id > msg.session_id) {
        players[pos].connected = false;
        return;
    }

    players[pos].session_id = msg.session_id;
    players[pos].direction = msg.turn_direction;
    players[pos].expected_no = msg.next_expected_event_no;
    players[pos].last_sent = current_us();

    if (msg.turn_direction != 0 && players[pos].ready == false)
        players[pos].ready = true;

}


// send message to client
void send_client(pollfd &sock, uint32_t event_no, std::vector<event> &events,
            sockaddr_in6 &client_address, uint32_t game_id,
            std::vector<uint8_t> &stringifed, std::vector<std::string> &event_string) {

    uint32_t i = event_no;
    message_stc msg;
    std::string to_send;

    while (i < events.size()) {

        // send maximal sized datagrams

        to_send = "";

        append_data<uint32_t>(game_id, to_send);

        for (; i < events.size(); i++) {

            if (stringifed[i] == 0) {

                event_string[i] = event_str(events[i]);;
                stringifed[i] = 1;
            }

            if (to_send.size() + event_string[i].size() > MAX_UDP_DATA)
                break;

            to_send += event_string[i];
        }

        send_string(sock, to_send, client_address);
    }
}

// send message to single client
void send_single_client(pollfd &sock, uint32_t event_no, std::vector<event> &events,
            sockaddr_in6 &client_address, uint32_t game_id) {

    std::vector<uint8_t> stringifed(events.size());
    std::vector<std::string> event_string(events.size());

    send_client(sock, event_no, events, client_address, game_id,
                stringifed, event_string);
}


// send message to all connected clients
void broadcast(uint32_t game_id, std::vector<event> &events,
                std::vector<player> &players, pollfd &sock) {

    std::vector<uint8_t> stringifed(events.size());

    std::vector<std::string> event_string(events.size());

    for (player p : players) {
        if (p.connected)
            send_client(sock, p.expected_no, events, p.address, game_id, \
                        stringifed, event_string);
    }
}


int receive_msg_cts(bool in_game, pollfd &sock, std::vector<player> &players,
                    std::vector<event> &events, uint32_t game_id = 0) {


    message_cts msg;
    sockaddr_in6 client_address;

    int checked_sock = check_socket(sock, in_game);

    if (checked_sock != 1)
        return checked_sock;

    if (!process_msg_cts(sock, client_address, msg))
        return 0;

    int pos = check_if_in_players(players, client_address, msg.player_name);

    // same name as different user
    if (pos == -1)
        return 0;

    if (pos == MAX_PLAYERS) {
        pos = try_find_slot(players);

    if (pos != MAX_PLAYERS)
        add_player(players, pos, msg, client_address);

    } else {

        update_player(players, pos, msg);
    }

    if (pos != MAX_PLAYERS && in_game) {

        send_single_client(sock, msg.next_expected_event_no, events,
                        client_address, game_id);
    }

    return 1;
}


void set_ids(std::vector<std::string> &player_names, std::vector<player> &players) {

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {
        players[i].id = MAX_PLAYERS;
    }

    for (uint32_t i = 0; i < player_names.size(); i++) {
        for (uint32_t j = 0; j < MAX_PLAYERS; j++) {
            if (players[j].name == player_names[i]) {
                players[j].id = i;
                break;
            }
        }
    }
}


void make_players_list(std::vector<std::string> &player_names,
            std::vector<player> &players) {

    uint32_t datagram_size = 4;

    player_names = std::vector<std::string>();

    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {

        if (players[i].connected && players[i].name != "") {
            datagram_size += players[i].name.size();

            if (datagram_size > MAX_UDP_DATA)
                break;

            player_names.push_back(players[i].name);
        }
    }

    std::sort(player_names.begin(), player_names.end());

    set_ids(player_names, players);
}


// correct directions of snakes
void set_directions(std::deque<snake> &snakes, std::vector<player> &players,
                    server_params &sp) {

    uint8_t id;

    for (uint32_t i = 0; i < snakes.size(); i++) {
        id = snakes[i].id;

        for (uint32_t j = 0; j < MAX_PLAYERS; j++) {
            if (players[j].id == id) {
                if (players[j].direction == 1) {
                    snakes[i].direction += sp.turn;
                    break;

                } else if (players[j].direction == -1) {
                    snakes[i].direction -= sp.turn;
                    break;

                }
            }
        }
    }
}


void zero_initial_turn(std::vector<player> players) {
    for (uint32_t i = 0; i < MAX_PLAYERS; i++) {
        players[i].direction = 0;
    }
}


int main(int argc, char *argv[]) {

    int res;

    server_params sp;

    sockaddr_in6 address;

    pollfd sock;

    uint64_t last_round;

    uint64_t round_time;

    std::vector<player> players (MAX_PLAYERS);

    std::vector<std::string> player_names;

    std::vector<event> empty_events;

    stats sta;

    bool in_game;


    if (!fill_server_params(sp, argc, argv))
        return 1;

    if (!establish_address(address, sp.port))
        return 1;

    if (!get_socket(sock))
        return 1;

    if (!bind_socket(sock, address))
        return 1;


    round_time = 1000000 / sp.speed;

    in_game = false;

    while(true) {

        res = receive_msg_cts(in_game, sock, players, empty_events);

        if (res == -1)
            return -1;

        update_stats(sta, players);

        if (sta.ready >= 2 && sta.ready + sta.inactive == sta.connected) {

            // game start

            in_game = true;

            make_players_list(player_names, players);

            zero_initial_turn(players);

            game_state gs = new_game(player_names, sp);

            broadcast(gs.game_id, gs.all_events, players, sock);

            last_round = current_us();

            while (true) {

                while (current_us() - last_round < round_time) {

                    res = 1;

                    while(res == 1)
                        res = receive_msg_cts(in_game, sock, players,
                            gs.all_events, gs.game_id);

                    if (res == -1)
                        return -1;

                }

                last_round += round_time;

                set_directions(gs.snakes, players, sp);

                round(gs, sp);

                broadcast(gs.game_id, gs.all_events, players, sock);

                if (gs.all_events.back().event_type == GAME_OVER)
                    break;
            }

            for (uint32_t i = 0; i < MAX_PLAYERS; i++) {
                players[i].direction = 0;
                players[i].ready = false;
                players[i].id = MAX_PLAYERS;
            }

            in_game = false;
        }
    }

    return 0;
}
