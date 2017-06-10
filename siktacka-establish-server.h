#ifndef SIKTACKA_ESTABLISH_SERVER
#define SIKTACKA_ESTABLISH_SERVER

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <netdb.h>

bool establish_address(sockaddr_in6 &address, int port);

bool get_socket(pollfd &sock);

bool bind_socket(pollfd &sock, sockaddr_in6 &address);

#endif /* SIKTACKA_ESTABLISH_SERVER */
