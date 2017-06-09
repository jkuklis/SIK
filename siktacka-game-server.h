#ifndef SIKTACKA_GAME_SERVER
#define SIKTACKA_GAME_SERVER

#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>
#include <list>
#include <set>
#include <deque>

struct spot {
    uint32_t x;
    uint32_t y;

    spot() {}

    spot(uint32_t x, uint32_t y) : x(x), y(y) {}
};


struct snake {
    uint32_t id;
    std::string player_name;
    long double x;
    long double y;
    long double direction;


    snake() {}

    snake(uint32_t id, std::string player_name, long double x, long double y,
        long double direction) :
                id(id),
                player_name(player_name),
                x(x),
                y(y),
                direction(direction) {}
};


struct game_state {
    uint32_t game_id;
    std::vector<snake> snakes;
    std::vector<event> all_events;
    std::vector<std::vector<uint8_t> > map;

    game_state() {}

    game_state(uint32 game_id, std::deque<snake> snakes,
        std::vector<event> all_events, std::vector<std::vector<uint8_t> > map) :
                game_id(game_id),
                snakes(snakes),
                all_events(all_events),
                map(map) {}
};


//uint32_t next_rand(uint32_t rand);

game_state new_game(std::vector<std::string> player_names, server_params &sp);

#endif /* SIKTACKA_GAME_SERVER */
