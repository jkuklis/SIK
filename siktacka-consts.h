#ifndef SIKTACKA_CONSTS
#define SIKTACKA_CONSTS

#include <string>
#include <limits>
#include <time.h>

const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 600;
const int DEFAULT_PORT = 12345;
const int DEFAULT_SPEED = 50;
const int DEFAULT_TURN = 6;

const int MIN_WIDTH = 1;
const int MAX_WIDTH = 4000;
const int MIN_HEIGHT = 1;
const int MAX_HEIGHT = 4000;
const int MIN_PORT = 1024;
const int MAX_PORT = 65535;
const int MIN_SPEED = 1;
const int MAX_SPEED = 200;
const int MIN_TURN = 1;
const int MAX_TURN = 200;
const int MIN_INT = std::numeric_limits<int>::min();
const int MAX_INT = std::numeric_limits<int>::max();

const std::string DEFAULT_HOST = "localhost";
const int DEFAULT_GUI_PORT = 12346;

// prefered 1
const int MIN_LENGTH = 0;
const int MAX_LENGTH = 64;

const int S6_ADDR_LEN = 16;

const int MAX_EVENT_TYPE = 3;
const int NEW_GAME = 0;
const int PIXEL = 1;
const int PLAYER_ELIMINATED = 2;
const int GAME_OVER = 3;

const int MAX_UDP_DATA = 512;
const int MAX_PLAYERS = 42;

#define POLY 0xedb88320

#endif /* SIKTACKA_CONSTS */
