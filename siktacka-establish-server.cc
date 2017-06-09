#include <iostream>
#include <string.h>

#include "siktacka-consts.h"
#include "siktacka-establish-server.h"

bool establish_address(sockaddr_in6 &address, int port) {
    address.sin6_family = AF_INET6;
	address.sin6_port = htons(port);
    
    for (int i = 0; i < S6_ADDR_LEN; i++) {
        address.sin6_addr.s6_addr[i] = 0;
    }

    return true;
}

bool get_socket(int &sock) {
    sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (sock < 0) {
        std::cout << "Failed to open socket\n";
        return false;
    }
    return true;
}

bool bind_socket(int &sock, sockaddr_in6 &address) {
    if (bind(sock, (sockaddr*) &address, (socklen_t) sizeof(address)) < 0) {
        std::cout << "Failed to bind socket\n";
        return false;
    }
    return true;
}
