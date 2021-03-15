#ifndef IRC_HEADER_H
#define IRC_HEADER_H

#include <stdint.h>

#include "mem-list.h"
#include "user.h"

typedef struct IrcMessage {
    MemoryNode* original;
    MemoryNode* copied;
    char *ptr;
    int from_fd;
    char* from;
    char* to;
    char* message;
    char* cmd;
    int hasError;
    char* error;
} IrcMessage;

typedef enum IrcError {
    IrcMessageTooLong = -1,
    IrcNameTooLong = -2,
    IrcToNameTooLong = -3,
    IrcInvalidCharacters = -4,
} IrcError;

#define PRIVMSG "PRIVMSG"
#define JOIN "JOIN"
#define PING "PING"
#define PONG "PONG"

void irc_new_fd(int fd);
void irc_parse_message(IrcMessage* msg);
void irc_print_message(IrcMessage* msg);
int irc_process_message(IrcMessage* msg);

#endif
