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

#include "irc.h"
#include "user.h"
#include "mem-list.h"

// This is the only port that works on every system guaranteed
//
#define ECHO_PORT 1337

#define MAX_LINE 1000
#define LISTENQ 1024

// Efficiency please?
// This will clearly  break at MAX_LINE
int read_line(int sock, MemoryNode* node) {
    size_t length = 0;
    char* buffer = node->data;
    int max_size = get_max_size();
    int found = 0;

    do {
        char data;
        int result = recv(sock, &data, 1, 0);

        if ((result <= 0) || (data == EOF)) {
            perror("Connection closed");
            return -1;
        }

        buffer[length] = data;
        length++;

        printf("res: %.*s\n", (int)length, buffer);

        found = length >= 2 && buffer[length - 2] == '\r' && buffer[length - 1] == '\n';
    } while (!found && length < max_size);

    if (found) {
        node->length = length;
        return 1;
    }

    return 0;
}


ssize_t writeline(int sockd, const void* vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char* buffer;

    buffer = vptr;
    nleft  = n;

    while (nleft > 0) {
        if ((nwritten = write(sockd, buffer, nleft)) <= 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return -1;
            }
        }
        nleft  -= nwritten;
        buffer += nwritten;
    }

    return n;
}

void read_from_socket(int conn) {
    printf("Awaiting message\n");
    MemoryNode* node = get_memory();
    int length = read_line(conn, node);

    if (length == -1) {
        exit(EXIT_FAILURE);
    }

    IrcMessage msg;
    MemoryNode* copied = get_memory();
    // TODO: Refactor me, but future you.  So not right now while you read
    // this. Or !so @polarmutex 
    //
    // ^-- tj paid actually money in subs for me to write this.
    memcpy(copied->data, node->data, node->length);
    msg.original = node;
    msg.copied = copied;
    msg.from_fd = conn;
    msg.hasError = 0;

    irc_parse_message(&msg);
    irc_print_message(&msg);

    if (irc_process_message(&msg)) {
        IrcUser** users = get_user_list();
        for (int i = 0; i < get_users_size(); ++i) {
            // TODO: This should definitely get semaphored out of its mind
            if (users[i]->from_fd == conn) {
                printf("Please remove me, but for testing purposes, this is nice, and continue here.\n");
            }

            writeline(users[i]->from_fd, msg.original->data, msg.original->length);
        }
    } else {
        printf("Closing down the connection %d\n", msg.from_fd);
        // detele the users?
        close(conn);
    }
}

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

        IrcUser** users = get_user_list();
        for (int i = 0; i < get_users_size(); ++i) {
            FD_SET(users[i]->from_fd, &active_fd_set);
        }

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
                    read_from_socket(i);
                }
            }
        }
    }
}

