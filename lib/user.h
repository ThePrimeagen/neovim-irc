#ifndef USER_HEADER_H
#define USER_HEADER_H

#include "mem-list.h"

typedef enum IrcConnectionState {
    IrcStateWaitingToJoin,
    IrcStateReady,
    IrcStateDisconnected,
} IrcConnectionState;

typedef enum UserState {
    UserStateWaitingForData,
    UserStatePartiallyRead,
    UserStateHasData,
    UserStateError,
} UserState;

typedef struct User {
    int from_fd;
    char* name;
    int relative_message_count;
    unsigned long long int last_pong_time;
    IrcConnectionState irc_state;
    UserState state;
    MemoryNode* scratch_data;
} User;

const char* user_state_to_string(IrcConnectionState state);
int insert_user(User* user);
User* create_user(int fd);
User* find_user(int fd);
void delete_user(User* user);
User** get_user_list();
int get_users_size();

// PUT THIS SOMEWHERE ELSE
long long current_timestamp();

#endif
