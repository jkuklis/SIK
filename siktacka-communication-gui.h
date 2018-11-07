#ifndef SIKTACKA_COMMUNICATION_GUI
#define SIKTACKA_COMMUNICATION_GUI


#include <string>
#include <vector>

#include "siktacka-communication-server.h"


// analyze new_game event data
bool decompose_new_game(std::string &event_data, uint32_t &maxx, uint32_t &maxy,
            std::vector<std::string> &players);


// analyze pixel event data
bool decompose_pixel(std::string &event_data, uint8_t &id,
            uint32_t &x, uint32_t &y);


// get information about players from event string
bool decompose_players(std::string &player_names,
            std::vector<std::string> &players);

#endif /* SIKTACKA_COMMUNICATION_GUI */
