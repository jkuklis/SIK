#ifndef SIKTACKA_CONSTS
#define SIKTACKA_CONSTS

#include <string>
#include <time.h>
#include <stdint.h>

// all constant values used by siktacka

const uint32_t DEFAULT_WIDTH = 800;
const uint32_t DEFAULT_HEIGHT = 600;
const uint32_t DEFAULT_PORT = 12345;
const uint32_t DEFAULT_SPEED = 50;
const uint32_t DEFAULT_TURN = 6;

const uint32_t MIN_WIDTH = 1;
const uint32_t MAX_WIDTH = 4000;
const uint32_t MIN_HEIGHT = 1;
const uint32_t MAX_HEIGHT = 4000;
const uint32_t MIN_PORT = 1024;
const uint32_t MAX_PORT = 65535;
const uint32_t MIN_SPEED = 1;
const uint32_t MAX_SPEED = 200;
const uint32_t MIN_TURN = 1;
const uint32_t MAX_TURN = 200;

const uint32_t MAX_UINT32 = 4294967295;

const std::string DEFAULT_HOST = "localhost";
const uint32_t DEFAULT_GUI_PORT = 12346;

const uint32_t MIN_LENGTH = 0;
const uint32_t MAX_LENGTH = 64;

const uint32_t S6_ADDR_LEN = 16;

const uint32_t MAX_EVENT_TYPE = 3;
const uint32_t NEW_GAME = 0;
const uint32_t PIXEL = 1;
const uint32_t PLAYER_ELIMINATED = 2;
const uint32_t GAME_OVER = 3;

const uint32_t MAX_UDP_DATA = 512;
const uint32_t MAX_PLAYERS = 42;

const uint32_t MAX_CS_DATAGRAM = 77;
const uint32_t BUFFER_SIZE = 80;

#define POLY 0xedb88320
#define PI 3.14159265

#endif /* SIKTACKA_CONSTS */
