#include "irc.h"
#include "user.h"
#include "mem-list.h"

void read_line(int sock, User* user);
ssize_t write_line(int sockd, char* vptr, size_t n);
int read_from_socket(int conn);
