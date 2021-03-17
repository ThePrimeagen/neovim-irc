#include <irc.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>

#include "user.h"

int irc_validate_string(char* str) {
    int len = strlen(str);
    int retVal = 1;
    for (int i = 0; retVal && i < len; ++i) {
        retVal = isalnum(str[i]) || ispunct(str[i]);
    }
    return retVal;
}

// that seems pretty safe
void add_name(User* user, char* name) {
    int len = strlen(name);
    user->name = (char*)malloc(sizeof(char) * len);
    if (!user->name) {
        printf("EVERYTHING IS BAD (add_name)\n");
        exit(69420);
    }
    memcpy(user->name, name, len);
}

void delete_user_by_fd(int fd) {
    delete_user(find_user(fd));
}

int irc_join(IrcMessage* msg) {
    User* usr = find_user(msg->from_fd);

    if (!usr) {
        printf("could not find the user...\n");
        return 0;
    }

    if (usr->irc_state != IrcStateWaitingToJoin) {
        printf("usr->state != IrcStateWaitingToJoin\n");
        return 0;
    }

    if (!irc_validate_string(msg->from)) {
        printf("irc_valide_string = Name is invalid\n");
        return 0;
    }

    printf("great success, The user is now joinededec:w :w \n");
    usr->irc_state = IrcStateReady;

    return 1;
}

char* parse_till_token(char* buffer, char* token, int untilEnd) {
    char* next = strstr(buffer, token);

    // I think this will work...
    if (next == NULL && untilEnd) {
        next = strstr(buffer, "\r\n");
    }

    if (next == NULL) {
        return NULL;
    }

    next[0] = 0;
    return ++next;
}

void irc_new_fd(int fd) {
    User* user = create_user(fd);
    user->irc_state = IrcStateWaitingToJoin;
    insert_user(user);
}

void irc_handle_pong(IrcMessage* msg) {
    User* usr = find_user(msg->from_fd);
    if (!usr) {
        // TODO:A does this happen?  Lets not put logging and pretend it doesn.
        return;
    }
    usr->last_pong_time = current_timestamp();
}

void irc_print_usr(User* usr) {
    printf("Usr(%d): %s\n", usr->from_fd, usr->name);
    printf("Count: %d Rel: %llu \n", usr->relative_message_count, current_timestamp() - usr->last_pong_time);
}

void irc_print_usr_by_msg(IrcMessage* msg) {
    User* usr = find_user(msg->from_fd);
    if (!usr) {
        printf("Usr(-1): Could not find User\n");
        return;
    }
    irc_print_usr(usr);
}

void irc_print_message(IrcMessage* out) {
    printf("DEBUG MESSAGE %p\n", out->cmd);
    printf("IRC Message %s\n", out->cmd);
    if (out->hasError) {
        printf("  Has Error: %s\n", out->error);
        return;
    }

    if (strncmp(PRIVMSG, out->cmd, 7) == 0) {
        printf("  From: %s -> %s\n", out->from, out->to);
        printf("  Message: %s\n", out->message);
    }
}

void irc_parse_join(char* buffer, IrcMessage* out) {
    // TODO(future me): Who has lots of motivation, imagen a world with
    // channels... think about it
    //
    // Currently, do nothing, just a join is sufficient....
    User* user = find_user(out->from_fd);
    add_name(user, out->from);
}

void irc_parse_message(IrcMessage* out) {
    // <:from> <cmd> <to> <:message>
    char* buffer = out->copied->data;
    char* next;

    if (buffer[0] == ':') {
        next = parse_till_token(buffer, " ", 0);
        if (next == NULL) {
            out->hasError = 1;
            out->error = "Message started with ':' but hand no space delimiter";
            return;
        }

        out->from = buffer;
        buffer = next;
    }

    next = parse_till_token(buffer, " ", 1);
    if (next == NULL) {
        out->hasError = 1;
        out->error = "Message does not have a command.";
        return;
    }
    out->cmd = buffer;
    buffer = next;

    if (strncmp(JOIN, out->cmd, 4) == 0) {
        irc_parse_join(buffer, out);
        return;
    }

    next = parse_till_token(buffer, " ", 0);
    // MA X IM UM Performance
    // Everything, to touch each other
    if (next == NULL) {
        out->hasError = 1;
        out->error = "Message does not have a to user.";
        return;
    }

    out->to = buffer;
    buffer = next;

    next = parse_till_token(buffer, "\r\n", 0);
    if (next == NULL) {
        out->hasError = 1;
        out->error = "There is no Registered Nurse in the message.";
        return;
    }

    out->message = buffer + 1;
}

// this is terrible code
int irc_process_message(IrcMessage* msg) {
    irc_print_usr_by_msg(msg);

    if (strcmp(PONG, msg->cmd) == 0) {
        irc_handle_pong(msg);
    } else if (strcmp(PING, msg->cmd) == 0) {
        return 0;
    } else if (strcmp(JOIN, msg->cmd) == 0) {
        printf("processing the join command\n");
        if (!irc_join(msg)) {
            return 0;
        }
    } else if (strcmp(PRIVMSG, msg->cmd) == 0) {
        irc_print_message(msg);
    }

    return 1;
}

const char* irc_state_to_string(IrcConnectionState state) {
    switch (state) {
        case IrcStateDisconnected:
            return "Disconnected";
        case IrcStateReady:
            return "Ready";
        case IrcStateWaitingToJoin:
        default:
            return "WaitingToJoin";
    }
}

