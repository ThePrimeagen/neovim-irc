#include <irc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
 
// If I keep ruining everything, please just turn this into a dlinked list
IrcUser* users[1337];
int users_size = 0;
 
int insert_user(IrcUser* user) {
    if (users_size == 1337) {
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

void remove_user(IrcUser* user) {
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
 
IrcUser* create_user(int fd) {
    IrcUser* user = (IrcUser*)malloc(sizeof(IrcUser));
    user->from_fd = fd;
    user->name = NULL;
     
    return user;
}

// that seems pretty safe
void add_name(IrcUser* user, char* name) {
    int len = strlen(name);
    user->name = (char*)malloc(sizeof(char) * len);
    memcpy(user->name, name, len);
}
 
IrcUser* find_user(int fd) {
    IrcUser* user = NULL;

    for (int i = 0; !user && i < users_size; ++i) {
        IrcUser* u = users[i];
        if (!u) {
            continue;
        }

        if (u->from_fd == fd) {
            user = u;
        }
    }

    return user;
}

void delete_user(IrcUser* user) {
    if (!user) {
        return;
    }

    if (user->name) {
        free(user->name);
    }
    free(user);
}

void delete_user_by_fd(int fd) {
    delete_user(find_user(fd));
}

char* parse_till_token(char* buffer, char* token) {
    char* next = strstr(buffer, token);

    if (next == NULL) {
        return NULL;
    }

    next[0] = 0;
    return ++next;
}
 
void irc_new_fd(int fd) {
    IrcUser* user = create_user(fd);
    user->state = IrcStateWaitingForJoin;
    insert_user(user);
}

void irc_close_fd(int fd) {
    IrcUser* user = find_user(fd);
    if (!user) {
        return;
    }

    remove_user(user);
    delete_user(user);
}

void irc_print_message(IrcMessage* out) {
    printf("IRC Message\n");
    if (out->hasError) {
        printf("  Has Error: %s\n", out->error);
        return;
    }
    printf("  From: %s -> %s\n", out->from, out->to);
    printf("  Message: %s\n", out->message);
}

void irc_parse_message(char* buffer, IrcMessage* out) {
    // <:from> <cmd> <to> <:message>

    char* next;

    if (buffer[0] == ':') {
        next = parse_till_token(buffer, " ");
        if (next == NULL) {
            out->hasError = 1;
            out->error = "Message started with ':' but hand no space delimiter";
            return;
        }

        out->from = buffer;
        buffer = next;
    }

    next = parse_till_token(buffer, " ");
    if (next == NULL) {
        out->hasError = 1;
        out->error = "Message does not have a command.";
        return;
    }
    out->cmd = buffer;
    buffer = next;

    next = parse_till_token(buffer, " ");
    if (next == NULL) {
        out->hasError = 1;
        out->error = "Message does not have a to user.";
        return;
    }
    out->to = buffer;
    buffer = next;

    next = parse_till_token(buffer, "\r\n");
        if (next == NULL) {
        out->hasError = 1;
        out->error = "There is no CRLF in the message.";
        return;
    if (next == NULL) {
        out->hasError = 1;
        out->error = "There is no CRLF in the message.";
        return;

    if (next == NULL) {
        out->hasError = 1;
        out->error = "There is no CRLF in the message.";
        return;
    }

    out->message = buffer + 1;
}

