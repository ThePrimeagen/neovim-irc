#include <irc.h>
#include <stdio.h>
#include <string.h>

char* parse_till_token(char* buffer, char* token) {
    char* next = strstr(buffer, token);

    if (next == NULL) {
        return NULL;
    }

    next[0] = 0;
    return ++next;
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

