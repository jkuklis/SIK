#ifndef SIKTACKA_COMMUNICATION_GUI
#define SIKTACKA_COMMUNICATION_GUI

// TODO gui accepts names, not ids

#include <string>
#include <vector>

#include "siktacka-communication-server.h"

bool message_to_gui(event &ev, std::string &str,
            std::string &name);


bool decompose_new_game(std::string &event_data, uint32_t &maxx, uint32_t &maxy,
            std::vector<std::string> &players);


bool decompose_pixel(std::string &event_data, uint8_t &id,
            uint32_t &x, uint32_t &y);


bool decompose_players(std::string &player_names,
            std::vector<std::string> &players);

#endif /* SIKTACKA_COMMUNICATION_GUI */
