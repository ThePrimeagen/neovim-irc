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
#include <unistd.h>
#include <sys/ioctl.h>

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

    int on = 1;
    int rc = setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(sock);
        exit(-1);
    }

    rc = ioctl(sock, FIONBIO, (char *)&on);
    if (rc < 0) {
        perror("ioctl() failed");
        close(sock);
        exit(-1);
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

    fd_set master_set;
    fd_set active_set;

    FD_ZERO(&master_set);
    FD_SET(sock, &master_set);

    int max_sd = sock;
    struct sockaddr_in clientname;

    do {

        memcpy(&active_set, &master_set, sizeof(master_set));

        printf("About to select %d\n", max_sd);
        int desc_count = select(max_sd + 1, &active_set, NULL, NULL, NULL);
        printf("desc_count %d\n", desc_count);
        if (desc_count < 0) {
            perror("select() has failed");
            exit(EXIT_FAILURE);
        } else if (desc_count == 0) {
            // Is this case for timeouts?
        }

        printf("Data is available now.\n");
        for (int i = 0; i <= max_sd; ++i) {
            printf("FD_ISSET(%d)\n", i);
            if (FD_ISSET(i, &active_set)) {
                printf("YES IT IS(%d)\n", i);
                if (i == sock) {

                    int conn;
                    do {
                        size_t size = sizeof(clientname);
                        conn = accept(sock, NULL, NULL);
                        if (conn < 0) {
                            if (errno != EWOULDBLOCK) {
                                perror("error calling accept()");
                                exit(EXIT_FAILURE);
                            }
                            continue;
                        }

                        rc = ioctl(conn, FIONBIO, (char *)&on);
                        if (rc < 0) {
                            perror("ioctl() failed INCOMING_CONNECTION.");
                            close(conn);
                            close(sock);
                            exit(-1);
                        }

                        irc_new_fd(conn);
                        printf("incoming connection %d\n", conn);
                        FD_SET(conn, &master_set);
                        if (conn > max_sd) {
                            max_sd = conn;
                        }
                    } while (conn != -1);

                } else {
                    int res = read_from_socket(i);
                    printf("res rform read_from_socket %d\n", res);
                    if (!res) {
                        User *user = find_user(i);
                        delete_user(user);
                        close(i);
                        FD_CLR(i, &master_set);
                        if (i == max_sd) {
                            while (FD_ISSET(max_sd, &master_set) == 0) {
                                max_sd -= 1;
                            }
                        }
                    }
                }
            }
        }
    } while (1);
}

