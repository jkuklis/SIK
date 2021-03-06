#ifndef SIKTACKA_COMMUNICATION_SERVER
#define SIKTACKA_COMMUNICATION_SERVER

#include <string>
#include <vector>
#include <string.h>
#include <stdint.h>

#include "siktacka-input-server.h"

uint32_t event_crc32(uint32_t len, uint32_t event_no, uint8_t event_type,
            std::string &event_data);

struct event {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type;
    std::string event_data;
    uint32_t crc32;

    event() {}

    event(uint32_t len, uint32_t event_no, uint8_t event_type,
                std::string &event_data) :
                len(len),
                event_no(event_no),
                event_type(event_type),
                event_data(event_data) {

        this->crc32 = event_crc32(len, event_no, event_type, event_data);
    }
};


// message structure, sent to client by server
struct message_stc {
    uint32_t game_id;
    std::vector<event> events;

    message_stc() {}

    message_stc(int32_t game_id, std::vector<event> &events) :
                game_id(game_id),
                events(events) {}
};


// message structure, sent to server by client
struct message_cts {
    uint64_t session_id;
    int8_t turn_direction;
    uint32_t next_expected_event_no;
    std::string player_name;

    message_cts() {}

    message_cts(uint64_t session_id, int8_t turn_direction,
        uint32_t next_expected_event_no, std::string &player_name) :
                session_id(session_id),
                turn_direction(turn_direction),
                next_expected_event_no(next_expected_event_no),
                player_name(player_name) {}
};


event event_new_game(std::vector<std::string> &player_names, uint32_t event_no,
            server_params &sp);


event event_pixel(uint8_t id, uint32_t x, uint32_t y, uint32_t event_no,
            server_params &sp);


event event_player_eliminated(uint8_t id, uint32_t event_no,
            server_params &sp);


event event_game_over(uint32_t event_no, server_params &sp);



std::string event_str (event &ev);

std::string message_cts_str (message_cts &msg);

uint32_t message_stc_str (message_stc &msg, std::string &str,
            uint32_t first = 0);



bool event_from_str(event &ev, std::string &str);

bool message_stc_from_str(std::string &str, message_stc &msg);

bool message_cts_from_str(std::string &str, message_cts &msg);



uint32_t new_game_str(uint32_t maxx, uint32_t maxy,
            std::vector<std::string> &player_names, std::string &result);

std::string pixel_str(uint8_t player_number, uint32_t x, uint32_t y);


uint64_t current_us();


#endif /* SIKTACKA_COMMUNICATION_SERVER */
