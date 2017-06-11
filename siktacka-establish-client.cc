#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <algorithm>
#include <assert.h>

#include "siktacka-consts.h"
#include "siktacka-establish-client.h"


bool establish_address(sockaddr_in6 &address, std::string host, int port,
            addrinfo *addr_result) {
    addrinfo addr_hints;

    memset(&addr_hints, 0, sizeof(addrinfo));

    if (getaddrinfo(host.c_str(), NULL, &addr_hints, &addr_result) != 0) {
        std::cout << "Failed to get address info\n";
        return false;
    }

    address.sin6_family = AF_INET6;
    memcpy(address.sin6_addr.s6_addr,
        ((sockaddr_in6*) addr_result->ai_addr)->sin6_addr.s6_addr, S6_ADDR_LEN);
    address.sin6_port = htons((uint16_t) port);

    return true;
}


bool get_socket(pollfd &sock) {
    sock.fd = -1;
    sock.events = POLLIN;
	sock.revents = 0;

    sock.fd = socket(PF_INET6, SOCK_DGRAM, 0);

    if (sock.fd < 0) {
        std::cout << "Failed to open server socket\n";
        return false;
    }
    return true;
}


bool get_socket_tcp(pollfd &sock, addrinfo *addr_result) {
    sock.fd = -1;
    sock.events = POLLIN;
    sock.revents = 0;

    if (connect(sock.fd, addr_result->ai_addr, addr_result->ai_addrlen) < 0) {
        std::cout << "Failed to open gui socket\n";
        return false;
    }
    return true;
}
