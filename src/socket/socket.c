#include "socket.h"

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

// Efficiency please?
// This will clearly  break at MAX_LINE
void read_line(int sock, User* user) {
    char* buffer = user->scratch_data->data;
    int max_size = get_max_size();
    int done = 0;

    size_t length = 0;
    if (user->state == UserStatePartiallyRead) {
        length = user->scratch_data->length;
    }

    do {
        char data;
        int result = recv(sock, &data, 1, 0);

        if (result == 0) {
            done = 1;
        } else if ((result <= 0) || (data == EOF)) {
            user->state = UserStateError;
            return;
        } else {
            printf("buffer[%zu] = %c\n", length, data);
            buffer[length] = data;
            length++;
            done = length >= 2 && buffer[length - 2] == '\r' &&
                   buffer[length - 1] == '\n';
        }

    } while (!done && length < max_size);

    user->scratch_data->length = length;
    if (done) {
        user->state = UserStateHasData;
    } else if (length == max_size) {
        user->state = UserStateError;
    } else {
        user->state = UserStatePartiallyRead;
    }
}

// SOLVE ME LATER, groooooowwwlll
// cc @polarmutex, solve all my problems.
ssize_t writeline(int sockd, const void* vptr, size_t n) {
    // WTF??
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

int read_from_socket(int conn) {
    printf("Awaiting message\n");
    User* usr = find_user(conn);
    read_line(conn, usr);

    if (usr->state == UserStateError) {
        return 0;
    } else if (usr->state == UserStatePartiallyRead) {
        return 1;
    }

    IrcMessage msg;
    MemoryNode* node = usr->scratch_data;
    usr->scratch_data = get_memory();

    MemoryNode* copy = get_memory();
    memcpy(copy->data, node->data, node->length);

    // TODO: Refactor me, but future you.  So not right now while you read
    // this. Or !so @polarmutex
    //
    // ^-- tj paid actually money in subs for me to write this.
    memcpy(copy->data, node->data, node->length);
    msg.original = node;
    msg.copied = copy;
    msg.from_fd = conn;
    msg.hasError = 0;

    irc_parse_message(&msg);
    irc_print_message(&msg);

    if (irc_process_message(&msg)) {
        User** users = get_user_list();
        for (int i = 0; i < get_users_size(); ++i) {
            // TODO: This should definitely get semaphored out of its mind
            if (users[i]->from_fd == conn) {
                printf("Please remove me, but for testing purposes, this is nice, and continue here.\n");
            }

            // TODO: This doesn't always work... yikes
            writeline(users[i]->from_fd, msg.original->data, msg.original->length);
        }
        return 1;
    }
    return 0;
}
