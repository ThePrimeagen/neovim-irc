typedef struct IrcMessage {
    char *ptr;
    int from_fd;
    char* from;
    char* to;
    char* message;
    char* cmd;
    int hasError;
    char* error;
} IrcMessage;

void irc_parse_message(char* buffer, IrcMessage* msg);
void irc_print_message(IrcMessage* msg);
