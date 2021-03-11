#ifndef USER_HEADER_H
#define USER_HEADER_H

typedef enum IrcConnectionState {
    IrcStateWaitingToJoin,
    IrcStateReady,
    IrcStateDisconnected,
} IrcConnectionState;

typedef struct IrcUser {
    int from_fd;
    char* name;
    int relative_message_count;
    unsigned long long int last_pong_time;
    IrcConnectionState state;
} IrcUser;

const char* user_state_to_string(IrcConnectionState state);
int insert_user(IrcUser* user);
void remove_user(IrcUser* user);
IrcUser* create_user(int fd);
IrcUser* find_user(int fd);
void delete_user(IrcUser* user);
IrcUser** get_user_list();
int get_users_size();

// PUT THIS SOMEWHERE ELSE
long long current_timestamp();

#endif
