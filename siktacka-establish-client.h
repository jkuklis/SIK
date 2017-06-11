#ifndef SIKTACKA_ESTABLISH_CLIENT
#define SIKTACKA_ESTABLISH_CLIENT

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

bool establish_address_udp(sockaddr_in6 &address, std::string host, uint32_t port);

bool get_socket_udp(pollfd &sock);

bool get_socket_tcp(pollfd &sock, addrinfo *addr_result);

bool establish_connection_tcp(sockaddr_in6 &address, std::string host, uint32_t port,
                            pollfd &sock);

#endif /* SIKTACKA_ESTABLISH_CLIENT */
