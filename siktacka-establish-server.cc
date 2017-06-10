#include <iostream>
#include <string.h>

#include "siktacka-consts.h"
#include "siktacka-establish-server.h"

bool establish_address(sockaddr_in6 &address, int port) {
    address.sin6_family = AF_INET6;
	address.sin6_port = htons(port);

    for (uint32_t i = 0; i < S6_ADDR_LEN; i++) {
        address.sin6_addr.s6_addr[i] = 0;
    }

    return true;
}

bool get_socket(pollfd &sock) {
    sock.fd = -1;
    sock.events = POLLIN;
	sock.revents = 0;

    sock.fd = socket(AF_INET6, SOCK_DGRAM, 0);

	if (sock.fd < 0) {
        std::cout << "Failed to open socket\n";
        return false;
    }
    return true;
}

// NOTE : maybe close sockets?

bool bind_socket(pollfd &sock, sockaddr_in6 &address) {
    if (bind(sock.fd, (sockaddr*) &address, (socklen_t) sizeof(address)) < 0) {
        std::cout << "Failed to bind socket\n";
        return false;
    }
    return true;
}
