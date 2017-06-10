#include <vector>

#include "siktacka-communication-server.h"
#include "siktacka-input.h"
#include "siktacka-consts.h"

// TODO check, if maxx etc have suitable length
// TODO templete constraints_info, now ints

bool get_value(std::string &data, uint32_t &value, constr_info &constr) {

    bool result;

    value = parse_number<uint32_t>(data);

    result = checked_constraints(value, constr, false);

    return result;
}


bool manage_value(std::string &data, std::string str, std::string name,
            int min_value, int max_value) {
    uint32_t value;
    constraints_info constr;

    if (data.length() != 4)
        return false;

    constr = constraints_info(name, min_value, max_value);

    if(!get_value(data, value, constr)
        return false;

    append_value<uint32_t>(value, str);

    return true;
}


bool append_players(std::string player_names, std::string &str) {

    std::vector<std::string> parts;

    parts = split_to_vector(player_names, '\0');

    if (parts.size() > MAX_PLAYERS)
        return false;

    for (std::string name : parts) {
        if (!check_player_name(name, false) || name == "")
            return false;
    }

    std::multiset<std::string> sorted_names (player_names.begin(),
                player_names.end());

    for (uint32_t i = 0; i < sorted_names.size(); i++) {
        if (parts[i] != sorted_names[i])
            return false;
    }

    for (std::string name : parts) {
        str += name;
        str += ' ';
    }

    return true;
}


bool new_game_msg(std::string &event_data, std::string &str) {

    std::vector<std::string> parts;

    str = "NEW_GAME ";

    parts = split_to_vector(event_data, ' ');

    if (parts.size() != 3)
        return false;

    if (!manage_value(parts[0], str, "maxx", MIN_WIDTH, MAX_WIDTH))
        return false;

    if (!manage_value(parts[1], str, "maxy", MIN_HEIGHT, MAX_HEIGHT))
        return false;

    if (!append_players(parts[2], str))
        return false;

    return true;
}


bool pixel_msg(std::string &event_data, std::string &str) {

    uint32_t x;
    uint32_t y;
    uint8_t player_id;
    std::vector<std::string> parts;

    str = "PIXEL ";

    parts = split_to_vector(event_data, ' ');

    if (parts.size() != 3)
        return false;

    if (parts[0].length() != 4 || parts[1].length() != 4
                || parts[2].length() != 1)
        return false;


    x = parse_number<uint32_t>(parts[0]);
    y = parse_number<uint32_t>(parts[1]);

    player_id = parse_number<uint8_t>(parts[2]);

    if (player_id > MAX_PLAYERS)
        return false;

    append_value<uint32_t>(x, str);
    append_value<uint32_t>(y, str);

    append_value<uint8_t>(player_id, str);

    return true;
}


bool player_eliminated_msg(std::string event_data, std::string str) {
    uint8_t player_id;

    str = "PLAYER_ELIMINATED ";

    if (event_data.length() != 1)
        return false;

    player_id = event_data[0];

    append_value<uint8_t>(player_id, str);

    return true;
}


bool message_to_gui(event &ev, std::string &str) {

    bool result;

    str = "";

    switch(ev.event_type) {

        case NEW_GAME:
            result = new_game_msg(ev.event_data, str);
            break;

        case PIXEL:
            result = pixel_msg(ev.event_data, str);
            break;

        case PLAYER_ELIMINATED:
            result = player_eliminated_msg(ev.event_data, str);
            break;

        default:
            result = false;
    }

    return result;
}
