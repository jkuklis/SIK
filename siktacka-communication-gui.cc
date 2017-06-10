#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

#include "siktacka-input.h"
#include "siktacka-consts.h"
#include "siktacka-communication-gui.h"

// TODO check, if maxx etc have suitable length
// TODO templete constraints, now ints


bool decompose_players(std::string &player_names,
            std::vector<std::string> &players) {

    players = split_to_vector(player_names, '\0');

    if (players.back() != "")
        return false;

    players.pop_back();

    for (std::string name : players) {

        if (!check_player_name(name, false) || name == "")
            return false;
    }

    std::vector<std::string> sorted_names (players.begin(), players.end());


    std::sort(sorted_names.begin(), sorted_names.end());

    for (uint32_t i = 0; i < players.size(); i++) {
        if (players[i] != sorted_names[i])
            return false;
    }

    return true;
}


bool decompose_new_game(std::string &event_data, uint32_t &maxx, uint32_t &maxy,
            std::vector<std::string> &players) {

    std::string player_names;

    if (event_data.size() < 12)
        return false;

    maxx = parse_number<uint32_t>(event_data, 0, 4);
    maxy = parse_number<uint32_t>(event_data, 4, 8);

    if (maxx < MIN_WIDTH || maxx > MAX_WIDTH)
        return false;

    if (maxy < MIN_HEIGHT || maxy > MAX_HEIGHT)
        return false;

    player_names = event_data.substr(8);

    if (!decompose_players(player_names, players))
        return false;

    return true;
}


bool decompose_pixel(std::string &event_data, uint8_t &id,
            uint32_t &x, uint32_t &y) {

    if (event_data.size() != 9)
        return false;

    id = parse_number<uint32_t>(event_data, 0, 1);
    x = parse_number<uint32_t>(event_data, 1, 5);
    y = parse_number<uint32_t>(event_data, 5, 9);

    return true;
}



bool message_to_gui(event &ev, std::string &str, std::string name) {

    bool result;

    str = "";

    switch(ev.event_type) {

        case NEW_GAME:
            //result = new_game_msg(ev.event_data, str);
            break;

        case PIXEL:
            //result = pixel_msg(ev.event_data, str, name);
            break;

        case PLAYER_ELIMINATED:
            //result = player_eliminated_msg(ev.event_data, str, name);
            break;

        default:
            result = false;
    }

    return result;
}
