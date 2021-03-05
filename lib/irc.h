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

typedef enum IrcConnectionState {
    IrcStateWaitingForJoin,
    IrcStateReady,
    IrcStateDisconnected,
} IrcConnectionState;
 
typedef struct IrcUser {
    int from_fd;
    char* name;
    int relative_message_count; 
    IrcConnectionState state;
} IrcUser;

typedef enum IrcError {
    IrcMessageTooLong = -1,
    IrcNameTooLong = -2,
    IrcToNameTooLong = -3,
    IrcInvalidCharacters = -4,
} IrcError;

void irc_new_fd(int fd);
void irc_close_fd(int fd);
void irc_parse_message(char* buffer, IrcMessage* msg);
void irc_print_message(IrcMessage* msg);
void irc_process_message(IrcMessage* msg);

