#ifndef SIKTACKA_ESTABLISH_CLIENT
#define SIKTACKA_ESTABLISH_CLIENT

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

bool establish_address(sockaddr_in6 &address, std::string host, int port,
            addrinfo *addr_result);

bool get_socket(pollfd &sock);

bool get_socket_tcp(pollfd &sock, addrinfo *addr_result);

#endif /* SIKTACKA_ESTABLISH_CLIENT */
