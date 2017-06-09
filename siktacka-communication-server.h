#ifndef SIKTACKA_COMMUNICATION_SERVER
#define SIKTACKA_COMMUNICATION_SERVER

#include <string>
#include <vector>

// TODO

uint32_t calc_crc32(uint32_t len, uint32_t event_no, uint8_t event_type,
            std::string event_data);

struct event {
    uint32_t len;
    uint32_t event_no;
    uint8_t event_type;
    std::string event_data;
    uint32_t crc32;

    event() {}

    event(uint32_t len, uint32_t event_no, uint8_t event_type,
        std::string event_data) :
                len(len),
                event_no(event_no),
                event_type(event_type),
                event_data(event_data),
                crc32(calc_crc32()) {}
};


// should broadcast be there?

void broadcast(event ev) {
    // TODO
}


struct message_stc {
    uint32_t game_id;
    std::vector<event> events;

    message_stc() {}

    message_stc(int32_t game_id, std::vector<event> events) :
                game_id(game_id),
                events(events) {}
};


struct message_cts {
    uint64_t session_id;
    int8_t turn_direction;
    uint32_t next_expected_event_no;
    std::string player_name;

    message_cts() {}

    message_cts(int64_t session_id, int8_t turn_direction,
        uint32_t next_expected_event_no, std::string player_name) :
                session_id(session_id),
                turn_direction(turn_direction),
                next_expected_event_no(next_expected_event_no),
                player_name(player_name) {}
};


std::string event_str (event ev);

std::string message_cts_str (message_cts msg);

std::string message_stc_str (message_stc msg);


std::string new_game_str(uint32_t maxx, uint32_t maxy,
            std::vector<std::string> player_names);

std::string pixel_str(uint8_t player_number, uint32_t x, uint32_t y);


uint64_t current_ms();

#endif /* SIKTACKA_COMMUNICATION_SERVER */