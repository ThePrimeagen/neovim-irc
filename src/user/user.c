#include "user.h"

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_USERS 1337

// If I keep ruining everything, please just turn this into a dlinked list
User* users[MAX_USERS];
int users_size = 0;

User** get_user_list() {
    return users;
}

int get_users_size() {
    return users_size;
}

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

int insert_user(User* user) {
    if (users_size == MAX_USERS) {
        return 0;
    }

    int i = 0;
    int inserted = 0;

    for (; inserted && i < users_size; ++i) {
        if (users[i] == NULL) {
            inserted = 1;
            users[i] = user;
        }
    }

    if (!inserted) {
        inserted = 1;
        users[users_size] = user;
    }

    if (i + 1 >= users_size) {
        users_size++;
    }

    return 1;
}

void remove_user(User* user) {
    int removed = 0;
    int i = 0;
    for (; removed && i < users_size; ++i) {
        if (users[i] != NULL && users[i]->from_fd == user->from_fd) {
            removed = 1;
            users[i] = NULL;
        }
    }

    if (i + 1 == users_size) {
        users_size--;
    }
}

User* create_user(int fd) {
    User* user = (User*)malloc(sizeof(User));
    if (!user) {
        printf("EVERYTHING IS BAD (create_user)\n");
        exit(69420);
    }

    user->from_fd = fd;
    user->name = NULL;
    user->scratch_data = get_memory();
    user->last_pong_time = current_timestamp();
    user->relative_message_count = 0;
    user->irc_state = IrcStateWaitingToJoin;
    user->state = UserStateWaitingForData;

    printf("Created a new user, %d - %p\n", fd, user->scratch_data);

    return user;
}

User* find_user(int fd) {
    User* user = NULL;

    for (int i = 0; !user && i < users_size; ++i) {
        User* u = users[i];
        if (!u) {
            continue;
        }

        if (u->from_fd == fd) {
            user = u;
        }
    }

    return user;
}

void delete_user(User* user) {
    if (!user) {
        return;
    }
    remove_user(user);

    if (user->name) {
        free(user->name);
    }

    release_memory(user->scratch_data);
    free(user);
}
