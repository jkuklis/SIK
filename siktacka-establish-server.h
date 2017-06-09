#ifndef SIKTACKA_ESTABLISH_SERVER
#define SIKTACKA_ESTABLISH_SERVER

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

bool establish_address(sockaddr_in6 &address, int port);

bool get_socket(int &sock);

bool bind_socket(int &sock, sockaddr_in6 &address);

#endif /* SIKTACKA_ESTABLISH_SERVER */
