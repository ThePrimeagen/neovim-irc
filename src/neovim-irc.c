#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include "socket.h"
#include "irc.h"
#include "user.h"
#include "mem-list.h"

// This is the only port that works on every system guaranteed
//
#define ECHO_PORT 1337

#define MAX_LINE 1000
#define LISTENQ 1024

int main() {
    int sock;
    struct sockaddr_in servaddr;
    char *endptr;

    short int port = (short int)ECHO_PORT;

    /*  Create the listening socket  */

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);

    /*  Bind our socket addresss to the
        listening socket, and call listen()  */

    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, LISTENQ) < 0) {
        fprintf(stderr, "ECHOSERV: Error calling listen()\n");
        exit(EXIT_FAILURE);
    }

    fd_set active_fd_set;
    FD_ZERO(&active_fd_set);
    struct sockaddr_in clientname;

    while (1) {
        FD_SET(sock, &active_fd_set);

        User** users = get_user_list();
        for (int i = 0; i < get_users_size(); ++i) {
            FD_SET(users[i]->from_fd, &active_fd_set);
        }

        printf("About to select\n");
        if (select (FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            printf("OHH MY GOODNSES %d\n", errno);
            exit(EXIT_FAILURE);
        }

        printf("Data is available now.\n");

        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET (i, &active_fd_set)) {
                if (i == sock) {
                    /* Connection request on original socket. */
                    size_t size = sizeof (clientname);
                    int conn = accept(sock, NULL, NULL);
                    if (conn < 0) {
                        fprintf(stderr, "ECHOSERV: Error calling accept()\n");
                        exit(EXIT_FAILURE);
                    }

                    // THis is a good idea ;) - Prime0
                    // Is it though? - Prime1
                    irc_new_fd(conn);
                } else {
                    if (read_from_socket(i) == 0) {
                        User* user = find_user(i);
                        delete_user(user);
                        close(i);
                    }
                }
            }
        }
    }
}

