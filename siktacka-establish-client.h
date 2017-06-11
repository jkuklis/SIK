#ifndef SIKTACKA_ESTABLISH_CLIENT
#define SIKTACKA_ESTABLISH_CLIENT

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

bool establish_address_udp(sockaddr_in6 &address, std::string host, int port);

bool get_socket_udp(pollfd &sock);

bool establish_connection_tcp(sockaddr_in6 &address, std::string host, int port,
                            pollfd &sock);

#endif /* SIKTACKA_ESTABLISH_CLIENT */
