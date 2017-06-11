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

#include "siktacka-input-client.h"
#include "siktacka-establish-client.h"
#include "siktacka-communication-server.h"
#include "siktacka-communication-gui.h"
#include "siktacka-input-server.h"

int check_sock(pollfd &sock) {

    int ret;
    sock.revents = 0;

    ret = poll(&sock, 1, 0);

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


int process_msg_stc(pollfd &sock, sockaddr_in6 &server_address,
                message_stc &msg_stc) {

    int len;
    int flags;
    socklen_t rcva_len;
    char server_buf[MAX_UDP_DATA];
    std::string server_str;

    flags = 0;
    rcva_len = (socklen_t) sizeof(server_address);

    len = recvfrom(sock.fd, server_buf, sizeof(server_buf), flags,
                (struct sockaddr *) &server_address, &rcva_len);

    server_str = std::string(server_buf, len);

    return message_stc_from_str(server_str, msg_stc);

}

bool send_to_gui(message_stc &msg_stc, std::vector<std::string> &player_names,
            client_params &cp, pollfd &sock) {

    bool result;
    uint32_t len;
    uint32_t x;
    uint32_t y;
    uint8_t id;
    std::string str;
    std::vector<std::string> names;

    for (event ev : msg_stc.events) {
        if (ev.event_no < cp.next_expected_event_no)
            continue;

        switch(ev.event_type) {
            case NEW_GAME:
                result = decompose_new_game(ev.event_data, x, y, names);
                if (!result)
                    break;

                str = "NEW_GAME ";

                append_value<uint32_t>(x, str);
                str += ' ';

                append_value<uint32_t>(y, str);
                str += ' ';

                for (uint32_t i = 0; i < names.size() - 1; i++) {
                    str += names[i];
                    str += ' ';
                }

                str += names.back();

                len = str.length();
                if (write(sock.fd, str.c_str(), len) < 0)
                    return 0;

                break;

            case PIXEL:
                result = decompose_pixel(ev.event_data, id, x, y);
                if (!result)
                    return 0;

                str = "PIXEL ";

                append_value<uint8_t>(id, str);
                str += ' ';

                append_value<uint32_t>(x, str);
                str += ' ';

                append_value<uint32_t>(y, str);

                len = str.length();
                if (write(sock.fd, str.c_str(), len) < 0)
                    return 0;

                break;

            case PLAYER_ELIMINATED:
                id = ev.event_data[0];

                str = "PLAYER_ELIMINATED ";

                str += id;

                len = str.length();
                if (write(sock.fd, str.c_str(), len) < 0)
                    return 0;

                break;

            case GAME_OVER:
                break;
        }

        cp.next_expected_event_no = ev.event_no + 1;

        if (ev.event_type == GAME_OVER)
            cp.next_expected_event_no = 0;
    }

    return 1;
}


int try_receive_server_msg(pollfd &server_sock, pollfd &ui_sock,
            sockaddr_in6 &server_address, std::vector<std::string> &player_names,
            uint32_t &game_id, client_params &cp) {

    message_stc msg_stc;

    int res;

    res = check_sock(server_sock);

    if (res != 1)
        return res;

    if (!process_msg_stc(server_sock, server_address, msg_stc))
        return 0;

    if (msg_stc.game_id < game_id)
        return 0;

    if (msg_stc.game_id > game_id) {
        if (msg_stc.events.front().event_type != NEW_GAME)
            return 0;
        else
            game_id = msg_stc.game_id;
    }

    send_to_gui(msg_stc, player_names, cp, ui_sock);

    return 1;
}


bool same_str(char *buf, std::string str) {
    bool result = true;

    for (uint32_t i = 0; i < str.size(); i++) {
        if (buf[i] != str[i]) {
            result = false;
            break;
        }
    }

    return result;
}


bool change_direction(char *buf, int8_t &direction) {
    std::string str;

    str = "LEFT_KEY_DOWN";

    if (same_str(buf, str))
        direction = -1;

    str = "LEFT_KEY_UP";

    if (same_str(buf, str))
        direction = 0;

    str = "RIGHT_KEY_DOWN";

    if (same_str(buf, str))
        direction = 1;

    str = "RIGHT_KEY_UP";

    if (same_str(buf, str))
        direction = 0;

    return true;
}


int try_receive_gui_msg(pollfd &sock, int8_t &direction) {
    int res;

    res = check_sock(sock);

    if (res != 1)
        return res;

    uint32_t buf_size = 20;

    char buf[buf_size];

    if (read(sock.fd, buf, buf_size) < 0)
        return 0;

    change_direction(buf, direction);

    return 1;
}


int main(int argc, char *argv[]) {

    int res;

    client_params cp;
    pollfd server_sock;
    pollfd ui_sock;
    sockaddr_in6 server_address;
    sockaddr_in6 ui_address;

    if (!fill_client_params(cp, argc, argv))
        return 1;

    print_client_params(cp);


    if (!establish_address_udp(server_address, cp.server_host, cp.server_port))
        return 1;

    if (!get_socket_udp(server_sock))
        return 1;

    if (!establish_connection_tcp(ui_address, cp.ui_host, cp.ui_port, ui_sock))
        return 1;

    int8_t direction = 0;

    uint64_t session_id = current_us();

    message_cts msg_cts;

    uint64_t last_sent = current_us();

    uint32_t game_id = 0;

    std::vector<std::string> players;

    while(true) {

        msg_cts = message_cts(session_id, direction, cp.next_expected_event_no,
                cp.player_name);

        std::string msg_str = message_cts_str(msg_cts);

        send_string(server_sock, msg_str, server_address);

        last_sent += 20000;

        while (current_us() - last_sent > 20000) {

            res = try_receive_server_msg(server_sock, ui_sock, server_address,
                                        players, game_id, cp);

            if (res == -1)
                return 1;

            res = try_receive_gui_msg(ui_sock, direction);

            if (res == -1)
                return 1;

            if (res == 1)
                std::cout << (int) direction << "\n";
        }
    }



    return 1;
}
