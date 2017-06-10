#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
//#include <stdio.h>
//#include <unistd.h>

#include "siktacka-input-server.h"
#include "siktacka-establish-server.h"
#include "siktacka-communication-server.h"
#include "siktacka-game-logic.h"

int main(int argc, char *argv[]) {
    server_params sp;
    int sock;
    sockaddr_in6 address;

    if (!fill_server_params(sp, argc, argv))
        return 1;
    print_server_params(sp);

    if (!establish_address(address, sp.port))
        return 1;

    if (!get_socket(sock))
        return 1;

    if (!bind_socket(sock, address))
        return 1;




    /*
    char buffer[100];
    struct sockaddr_in6 client_address;

    socklen_t snda_len = (socklen_t) sizeof(client_address);
    socklen_t rcva_len = (socklen_t) sizeof(client_address);
    int flags = 0; // we do not request anything special
    int len = recvfrom(sock, buffer, sizeof(buffer), flags,
            (struct sockaddr *) &client_address, &rcva_len);
    */


    return 0;
}
