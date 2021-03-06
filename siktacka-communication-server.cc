
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <time.h>

#include "siktacka-communication-server.h"
#include "siktacka-consts.h"
#include "siktacka-input.h"


// current microseconds from 1970-01-01 00:00:00 +0000 (UTC)
uint64_t current_us() {

    timeval start;
    uint64_t result;

    gettimeofday(&start, NULL);

    result = 1000000 * start.tv_sec + start.tv_usec;

    return result;
}


// strange magic to calculate a kind of checksum
// polynomial is taken from the standard incorporated within Ethernet
uint32_t crc32(const unsigned char *buf, size_t len) {
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

// 'checksum' of event's fields, without crc32 itself
uint32_t event_crc32(uint32_t len, uint32_t event_no, uint8_t event_type,
            std::string &event_data) {

    uint32_t result;

    std::string buf = "";

    append_data<uint32_t>(len, buf);
    append_data<uint32_t>(event_no, buf);
    buf += (uint8_t) event_type;
    buf += event_data;

    unsigned char buf_crc[4 + len];

    for (uint32_t i = 0; i < 4 + len; i++) {
        buf_crc[i] = (unsigned char) buf[i];
    }

    result = crc32(buf_crc, len + 4);

    return result;
}


event event_new_game(std::vector<std::string> &player_names, uint32_t event_no,
            server_params &sp) {

    uint32_t len;
    event result;

    result.event_no = event_no;

    result.event_type = NEW_GAME;

    len = new_game_str(sp.width, sp.height, player_names, result.event_data);

    result.len = len + 5;

    result.crc32 = event_crc32(result.len, result.event_no, result.event_type,
                result.event_data);

    return result;
}


event event_pixel(uint8_t id, uint32_t x, uint32_t y, uint32_t event_no,
            server_params &sp) {

    event result;
    uint32_t len;

    std::string event_data;

    event_data = pixel_str(id, x, y);

    len = 4 + 1 + 9;

    result = event(len, event_no, PIXEL, event_data);

    return result;
}


event event_player_eliminated(uint8_t id, uint32_t event_no,
            server_params &sp) {

    event result;
    uint32_t len;

    std::string event_data = "";

    event_data += id;

    len = 4 + 1 + 1;

    result = event(len, event_no, PLAYER_ELIMINATED, event_data);

    return result;
}


event event_game_over(uint32_t event_no, server_params &sp) {

    event result;
    uint32_t len = 5;

    std::string event_data = "";

    result = event(len, event_no, GAME_OVER, event_data);

    return result;
}


std::string event_str(event &ev) {

    std::string result;

    append_data<uint32_t>(ev.len, result);
    //result += ' ';

    append_data<uint32_t>(ev.event_no, result);
    //result += ' ';

    result += ev.event_type;
    //result += ' ';

    result += ev.event_data;
    //result += ' ';

    append_data<uint32_t>(ev.crc32, result);

    return result;
}


uint32_t message_stc_str(message_stc &msg, std::string &str,
            uint32_t first) {

    uint32_t i;
    uint32_t count;
    std::string ev_str;

    append_data<uint32_t>(msg.game_id, str);

    // note: iteration starts from event with number first
    for (i = first; i < msg.events.size(); i++) {
        ev_str = event_str(msg.events[i]);

        // if so, after appending another ev_str string to send would become
        // too long to be sent by our server
        if (str.length() + ev_str.length() > MAX_UDP_DATA)
            break;

        str += ev_str;
    }

    // how many events have been included in this message
    count = i - first;

    return count;
}


std::string message_cts_str (message_cts &msg) {

    std::string result;

    append_data<uint64_t>(msg.session_id, result);
    //result += ' ';

    result += msg.turn_direction;
    //result += ' ';

    append_data<uint32_t>(msg.next_expected_event_no, result);
    //result += ' ';

    result += msg.player_name;

    return result;
}


uint32_t new_game_str(uint32_t maxx, uint32_t maxy,
            std::vector<std::string> &player_names, std::string &result) {

    result = "";

    uint32_t len = 8;

    append_data<uint32_t>(maxx, result);
    //result += ' ';

    append_data<uint32_t>(maxy, result);
    //result += ' ';

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
    //result += ' ';

    append_data<uint32_t>(x, result);
    //result += ' ';

    append_data<uint32_t>(y, result);

    return result;
}


// correct data lengths for events
bool correct_data(uint32_t len, uint8_t event_type, std::string &event_data) {
    switch(event_type) {
        case NEW_GAME:
            return true;

        case PIXEL:
            return (len == 14);

        case PLAYER_ELIMINATED:
            return (len == 6);

        case GAME_OVER:
            return (len == 5);

        default:
            return false;
    }
}


bool event_from_str(event &ev, std::string &str) {

    if (str.length() < 9)
        return false;


    uint32_t len = parse_number<uint32_t>(str, 0, 4);

    uint32_t event_no = parse_number<uint32_t>(str, 4, 8);

    uint8_t event_type = parse_number<uint8_t>(str, 8, 9);

    std::string event_data = std::string(str, 9, len - 5);

    if (!correct_data(len, event_type, event_data))
        return false;

    ev = event(len, event_no, event_type, event_data);

    return true;
}


bool message_stc_from_str(std::string &str, message_stc &msg) {

    event ev;
    uint32_t next = 4;
    uint32_t size = str.size();
    uint32_t len;
    std::string event_str;

    if (str.size() < 4)
        return false;

    msg.game_id = parse_number<uint32_t>(str, 0, 4);

    while (next < size) {

        len = parse_number<uint32_t>(str, next, next + 4);

        event_str = std::string(str, next, len + 8);

        if (event_str.length() != len + 8) {
            return false;
        }

        if (!event_from_str(ev, event_str)) {
            return false;
        }

        if (ev.crc32 != event_crc32(ev.len, ev.event_no, ev.event_type,
                    ev.event_data)) {
            break;
        }

        if (ev.event_type > MAX_EVENT_TYPE) {
            continue;
        }

        next += len + 8;

        msg.events.push_back(ev);
    }

    if (next != size)
        return false;

    return true;
}


bool message_cts_from_str(std::string &str, message_cts &msg) {

    if (str.length() < 13)
        return false;

    msg.session_id = parse_number<uint64_t>(str, 0, 8);

    msg.turn_direction = parse_number<int8_t>(str, 8, 9);

    if (msg.turn_direction < -1 || msg.turn_direction > 1)
        return false;

    msg.next_expected_event_no = parse_number<uint32_t>(str, 9, 13);

    msg.player_name = str.substr(13);

    if (!check_player_name(msg.player_name)) {
        return false;
    }

    return true;
}
