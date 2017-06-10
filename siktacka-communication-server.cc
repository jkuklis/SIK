
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <time.h>

#include "siktacka-communication-server.h"
#include "siktacka-consts.h"
#include "siktacka-input.h"

// TODO ?
// string copy constructor


uint64_t current_ms() {
    timeval start;
    uint64_t result;

    gettimeofday(&start, NULL);

    result = 1000000 * start.tv_sec + start.tv_usec;

    return result;
}


uint32_t crc32(const unsigned char *buf, size_t len)
{
    uint32_t crc = 0;

    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
    return ~crc;
}


uint32_t event_crc32(uint32_t len, uint32_t event_no, uint8_t event_type,
            std::string event_data) {

    uint32_t result;

    std::string buf = "";

    append_data<uint32_t>(len, buf, false);
    append_data<uint32_t>(event_no, buf, false);
    //append_data<uint8_t>(event_type, buf, false);
    //append_data<std::string>(event_data, buf, false, len - 5)
    buf += (uint8_t) event_type;
    buf += event_data;

    return crc32(buf, len + 4);
}


// TODO server_params should have a broadcaster class
event event_new_game(std::vector<std::string> player_names, uint32_t event_no,
            server_params &sp) {

    uint32_t len;
    event result;

    result.event_no = event_no;

    result.event_type = NEW_GAME;

    len = new_game_str(sp.width, sp.height, player_names, result.event_data);

    result.len = len;

    // TODO broadcast(sp.broadcaster);

    return result;
}


event event_pixel(uint8_t id, uint32 x, uint32 y, uint32_t event_no,
            server_params &sp) {

    event result;

    result.event_no = event_no;

    result.event_type = PIXEL;

    result.event_data = pixel_str(id, x, y);

    result.len = 4 + 1 + 9;

    // broadcast(sp.broadcaster);

    return result;
}


event event_player_eliminated(uint8_t id, uint32_t event_no,
            server_params &sp) {

    event result;

    result.event_no = event_no;

    result.event_type = PLAYER_ELIMINATED;

    result.event_data = "";

    result.event_data += id;

    result.len = 4 + 1 + 1;

    // broadcast(sp.broadcaster);

    return result;
}


event event_game_over(uint32_t event_no, server_params &sp) {

    event result;

    result.event_no = event_no;

    result.event_type = GAME_OVER;

    result.event_data = "";

    // broadcast(sp.broadcaster);

    return result;
}


std::string event_str(event ev) {

    std::string result;

    append_data<uint32_t>(ev.len, result);
    result += ' ';

    append_data<uint32_t>(ev.event_no, result);
    result += ' ';

    result += ev.event_type;
    result += ' ';

    result += ev.event_data;
    result += ' ';

    append_data<uint32_t>(ev.crc32, result);

    return result;
}


uint32_t message_stc_str(message_stc msg, std::string &str,
            uint32_t first = 0) {

    uint32_t i;
    uint32_t count;
    std::string ev_str;

    append_data<uint32_t>(msg.game_id, str);

    // TODO server must check if with new game
    // there should be fewer players (512 B)

    for (i = first; i < msg.events.size(); i++) {
        ev_str = event_str(msg.events[i]);

        if (str.length() + ev_str.length() > MAX_UDP_DATA)
            break;

        result += ' ';
        result += event_str(ev);
    }

    count = i - first;

    return count;
}


std::string message_cts_str (message_cts msg) {

    std::string result;

    append_data<uint64_t>(msg.session_id, result);
    result += ' ';

    result += msg.turn_direction;
    result += ' ';

    append_data<uint32_t>(msg.next_expected_event_no, result);
    result += ' ';

    result += msg.player_name;

    return result;
}


uint32_t new_game_str(uint32_t maxx, uint32_t maxy,
            std::vector<std::string> player_names, std::string result) {

    result = "";

    uint32_t len = 0;

    append_data<uint32_t>(maxx, result);
    result += ' ';

    append_data<uint32_t>(maxy, result);
    result += ' ';

    for (std::string name : player_names) {
        result += name;
        result += '\0';

        len += name.length() + 1;
    }

    return len;
}


std::string pixel_str(uint8_t player_number, uint32_t x, uint32_t y) {

    std::string result;

    result += player_number;
    result += ' ';

    append_data<uint32_t>(x, result);
    result += ' ';

    append_data<uint32_t>(y, result);

    return result;
}


bool event_from_str(event &ev, std::string &str) {

    uint32_t len = parse_number<uint32_t>(str, 0, 4);

    // TODO
    // check, if len correct

    uint32_t event_no = parse_number<uint32_t>(str, 4, 8);

    uint8_t event_type = parse_number<uint8_t>(str, 8, 9);

    if (event_type > MAX_EVENT_TYPE)
        return false;

    std::string event_data = std::string(str, 9, len - 5);

    // TODO
    // check, if correct names data

    uint32_t crc32 = parse_number<uint32_t>(str, len, 4);

    ev = event(len, event_no, event_type, event_data);

    if (crc32 != ev.crc32)
        return false;

    return true;
}


bool message_stc_from_str(std::string &str, message_stc &msg) {

    event ev;
    std::vector<std::string> parts;

    parts = split_to_vector(str, ' ');

    msg.game_id = parse_number<uint32_t>(parts[0]);

    // as long as correct messages
    for (uint32_t i = 1; i < parts.size(); i++) {
        if (!event_from_str(ev, parts[i])) {
            break;
        }

        msg.push_back(ev);
    }

    return true;
}


bool message_cts_from_str(std::string &str, message_cts &msg) {

    std::vector<std::string> parts;

    parts = split_to_vector(str, ' ');

    if (parts.size() < 0 || parts.size() > 4)
        return false;

    msg.session_id = parse_number<uint64_t>(parts[0]);

    msg.turn_direction = parse_number<int8_t>(parts[1]);

    if (msg.turn_direction < -1 || msg.turn_direction > 1)
        return false;

    msg.next_expected_event_no = parse_number<uint32_t>(parts[2]);

    msg.player_name = parts[3];

    if (!check_player_name(msg.player_name)) {
        return false;
    }

    return true;
}
