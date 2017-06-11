#include <iostream>
#include <string>
#include <stdint.h>
#include <set>
#include <list>
#include <deque>
#include <math.h>

#include "siktacka-input-server.h"
#include "siktacka-game-logic.h"
#include "siktacka-consts.h"

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

    rand(coord, sp);

    coord %= modulus;

    coord_fl = (long double)(coord) + correction;

    return coord_fl;
}


void place_snake(snake &player, server_params &sp) {

    long double x, y;
    long double direction;
    snake result;

    x = rand_range(sp.width, 0.5, sp);
    y = rand_range(sp.height, 0.5, sp);

    direction = rand_range(360, 0, sp);

    player.x = x;
    player.y = y;
    player.direction = direction;
}



// TODO inline ?

inline spot map_position(snake &player) {
    return spot((uint32_t)(player.x), (uint32_t)(player.y));
}


inline bool is_free(spot position, std::vector<std::vector<uint8_t> > &map,
            server_params &sp) {
    return (position.x < sp.width && position.y < sp.height
                && map[position.y][position.x] == 0);
}


void make_map(game_state &gs, server_params &sp) {
    gs.map.resize(sp.height);

    for (uint32_t i = 0; i < sp.height; i++) {

        gs.map[i].resize(sp.width, 0);
    }
}



void add_snake(std::string &name, uint8_t &player_id, game_state &gs) {
    snake player (player_id, name, 0, 0, 0);

    gs.snakes.push_back(player);

    player_id ++;
}


void add_snakes(std::vector<std::string> player_names, game_state &gs) {

    uint8_t player_id = 0;

    // server should sort players
    //std::multiset<std::string> names(player_names.begin(), player_names.end());

    for (std::string name : player_names) {

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

    ev = event_new_game(player_names, 0, sp);

    gs.all_events.push_back(ev);
}


void add_pixel_map(uint32_t player_id, spot position, game_state &gs,
                server_params &sp) {

    event ev;

    gs.map[position.y][position.x] = 1;

    ev = event_pixel(player_id, position.x, position.y,
                gs.all_events.size(), sp);

    gs.all_events.push_back(ev);
}


void eliminate_player(uint32_t player_id, game_state &gs, server_params &sp) {
    event ev;

    ev = event_player_eliminated(player_id, gs.all_events.size(), sp);

    gs.all_events.push_back(ev);
}


void end_game(game_state &gs, server_params &sp) {
    event ev;

    ev = event_game_over(gs.all_events.size(), sp);

    gs.all_events.push_back(ev);
}


game_state new_game(std::vector<std::string> player_names, server_params &sp) {

    game_state gs;
    snake player;
    spot player_spot;

    init_game(player_names, gs, sp);

    for (auto it = gs.snakes.begin(); it != gs.snakes.end(); ) {
        place_snake((*it), sp);

        player = *it;

        player_spot = map_position(player);

        if (is_free(player_spot, gs.map, sp)) {

            add_pixel_map(player.id, player_spot, gs, sp);

            ++it;

        } else {
            eliminate_player(player.id, gs, sp);

            it = gs.snakes.erase(it);

            if (gs.snakes.size() <= 1) {

                end_game(gs, sp);

                break;
            }
        }
    }

    return gs;
}


void move_snake(snake &player) {
    player.x += cos(player.direction * PI / 180);
    player.y += sin(player.direction * PI / 180);
}


void round(game_state &gs, server_params &sp, std::vector<int8_t> &dir_table) {

    snake player;
    spot before;
    spot after;

    for (auto it = gs.snakes.begin(); it != gs.snakes.end(); ) {

        spot before = map_position(*it);

        switch(dir_table[it->id]) {

            case -1:
                it->direction -= sp.turn;
                break;

            case 1:
                it->direction += sp.turn;
                break;
        }

        move_snake(*it);

        spot after = map_position(*it);

        if (before.x == after.x && before.y == after.y)
            continue;

        if (is_free(after, gs.map, sp)) {

            add_pixel_map(it->id, after, gs, sp);

            ++it;

        } else {

            eliminate_player(it->id, gs, sp);

            it = gs.snakes.erase(it);

            if (gs.snakes.size() <= 1) {

                end_game(gs, sp);

                break;
            }
        }
    }
}
