#include <stdbool.h>
#include <irc.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char* parse_till_token(char* buffer, char* token) {
    char* next = strstr(buffer, token);

    if (next == NULL) {
        return NULL;
    }

    next[0] = 0;
    return ++next;
}

bool is_alpha_numeric_punc(const char* msg) {
    bool alpha_num_chars = true;
    int len = strlen(msg);
    for (int i = 0; alpha_num_chars && i < len; ++i) {
        alpha_num_chars = isalnum(msg[i]) || ispunct(msg[i]);
    }

    return alpha_num_chars;
}

int validate(IrcMessage* msg) {
    if (strlen(msg->from) > 15) {
        return IrcNameTooLong;
    }

    if (!is_alpha_numeric_punc(msg->from) || is_alpha_numeric_punc(msg->message) || 
            is_alpha_numeric_punc(msg->to)) {

        return IrcInvalidCharacters;
    }

    if (strlen(msg->to) > 15) {
        return IrcToNameTooLong;
    }

    return 1;
}

void irc_process_message(IrcMessage* msg) {
    if (strcmp("PONG", msg->cmd) == 0) {
        // Check to see if we sent a pong, if not, kick the connection
    } else if (strcmp("PING", msg->cmd) == 0) {
        // Boot the user
    } else if (strcmp("JOIN", msg->cmd) == 0) {
        // Basic requirements of having an alpha name.
    } else if (strcmp("PRIVMSG", msg->cmd) == 0) {
        irc_print_message(msg);
    }
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
    }

    out->message = buffer + 1;
}

