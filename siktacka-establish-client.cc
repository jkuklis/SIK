#include <iostream>
#include <string.h>

#include "siktacka-consts.h"
#include "siktacka-establish-client.h"


bool establish_address(sockaddr_in6 &address, std::string host, int port) {
    addrinfo server_addr_hints;
    addrinfo *server_addr_result;

    memset(&server_addr_hints, 0, sizeof(addrinfo));

    if (getaddrinfo(host.c_str(), NULL, &server_addr_hints, &server_addr_result) != 0) {
        std::cout << "Failed to get address info\n";
        return false;
    }

    address.sin6_family = AF_INET6;
    memcpy(address.sin6_addr.s6_addr,
        ((sockaddr_in6*) server_addr_result->ai_addr)->sin6_addr.s6_addr, S6_ADDR_LEN);
    address.sin6_port = htons((uint16_t) port);

    freeaddrinfo(server_addr_result);

    return true;
}


bool get_socket(int &sock) {
    sock = socket(PF_INET6, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cout << "Failed to open socket\n";
        return false;
    }
    return true;
}
