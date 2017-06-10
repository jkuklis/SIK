
#include <iostream>
#include <string>
#include <stdint.h>
#include <set>
#include <list>
#include <deque>

#include "siktacka-input-server.h"
#include "siktacka-game-logic.h"
#include "siktacka-consts"

// TODO event arguments

uint32_t next_rand(uint32_t rand) {
    uint64_t result = (uint64_t) rand * 279470273;

    result %= 4294967291;

    return (uint32_t) result;
}


void rand(uint32_t &var, server_params &sp) {
    var = sp.random;

    sp.random = next_rand(sp.random);
}

long double rand_range(uint32_t modulus, long double correction, server_params &sp) {
    uint32_t coord;
    long double coord_fl;

    rand(coord);

    coord %= modulus;

    coord_fl = (long_double)(coord) + correction;

    return coord_fl;
}

snake place_snake(std::name, server_params &sp) {

    long double x, y;
    long double direction;
    snake result;

    x = rand_range(sp.width, 0.5, sp);
    y = rand_range(sp.height, 0.5, sp);

    direction = rand_range(360, 0, sp);

    result = snake(name, x, y, direction);

    return result;
}

// TODO inline ?

inline spot map_position(snake player) {
    return spot((uint32_t)(player.x), (uint32_t)(player.y));
}


inline bool is_free(spot position, std::vector<std::vector<uint8_t> > &map) {
    return (map[spot.y][spot.x] == 0);
}


void make_map(game_state &gs, server_params &sp) {
    gs.map.resize(sp.height, 0);

    for (int i = 0; i < sp.height; i++) {

        gs.map[i].resize(sp.width, 0);
    }
}


void add_snake(std::string name, uint8 &id, game_state &gs) {
    snake player = (player_id, name, 0, 0, 0);

    gs.snakes.push_back(player);

    player_id ++;
}


void add_snakes(std::vector<std::string> names, game_state &gs) {

    uint8_t player_id = 0;

    std::multiset<std::string> names(player_names.begin(), player_names.end());

    for (std::string name : names) {

        // TODO - player list should not contain empty names
        // if (name != "")
        add_snake(name, player_id, gs);
    }
}


void init_game(std::vector<std::string> player_names, game_state &gs,
            server_params &sp) {
    event ev;

    rand(gs.game_id, sp);

    make_map(gs, sp);

    add_snakes(player_names, gs);

    ev = event_new_game(player_names, gs.all_events.size(), sp);

    gs.all_events.push_back(ev);
}


void add_player(snake player, spot position, game_state &gs) {

    event ev;

    gs.snakes.push_back(player);

    gs.map[position.y][position.x] = 1;

    ev = event_pixel();

    gs.all_events.push_back(ev);
}


void eliminate_player(uint32_t player_id, game_state &gs) {
    event ev;

    ev = event_player_eliminated();

    gs.all_events.push_back(ev);
}


game_state new_game(std::vector<std::string> player_names, server_params &sp) {

    game_state gs;
    snake player;
    spot player_spot;

    init_game(player_names, gs, sp);

    for (std::string name : player_names) {
        player = place_snake(name, sp);

        player_spot = map_position(player);

        if (is_free(player_spot, gs.map)) {

            add_player(player.id, player_spot, gs);

        } else {

            eliminate_player(player.id, gs);
        }
    }

    return gs;
}
