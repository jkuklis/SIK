#include "siktacka-communication-server.h"

#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <time.h>

uint32_t calc_crc32(uint32_t len, uint32_t event_no, uint8_t event_type,
            std::string event_data) {
    return 0;
}

template <typename T>
void append_data(T data, std::string &str) {
    int bytes = sizeof(T);

    uint8_t split[bytes];

    memcpy(split, &(data), bytes);

    for (int i = 0; i < bytes; i++) {
        str += split[bytes - 1 - i];
    }
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


std::string message_stc_str(message_stc msg) {

    std::string result;

    append_data<uint32_t>(msg.game_id, result);

    for (event ev : msg.events) {
        result += ' ';
        result += event_str(ev);
    }

    return result;
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


std::string new_game_str(uint32_t maxx, uint32_t maxy,
            std::vector<std::string> player_names) {

    std::string result;

    append_data<uint32_t>(maxx, result);
    result += ' ';

    append_data<uint32_t>(maxy, result);
    result += ' ';

    int last = player_names.size() - 1;

    for (int i = 0; i < last; i++) {
        result += player_names[i];
        result += ' ';
    }

    if (!player_names.empty())
        result += player_names[last];

    return result;
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


uint64_t current_ms() {
    timeval start;
    gettimeofday(&start, NULL);
    uint64_t result = 1000 * start.tv_sec + start.tv_usec;
    return result;
}

/*
int main () {
    std::string s = "abc";
    message_cts msg(1, 23, 1232, s);

    std::string str = message_cts_str(msg);

    for (int i = 0; i < str.length(); i++) {
        std::cout << (int) str[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
*/
