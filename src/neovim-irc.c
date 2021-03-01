#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <irc.h>

// This is the only port that works on every system guaranteed
#define ECHO_PORT 42069

#define MAX_LINE 1000
#define LISTENQ 1024

// Efficiency please?
// This will clearly  break at MAX_LINE
int read_line(int sock, char* buffer) {
    size_t length = 0;

    while (1) {
        char data;
        int result = recv(sock, &data, 1, 0);

        if ((result <= 0) || (data == EOF)) {
            perror("Connection closed");
            return -1;
        }

        buffer[length] = data;
        length++;

        printf("res: %.*s\n", (int)length, buffer);

        if (length >= 2 && buffer[length - 2] == '\r' && buffer[length - 1] == '\n') {
            break;
        }
    }

    return length;
}


/*
ssize_t writeline(int sockd, const void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 ) {
        if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
            if ( errno == EINTR )
                nwritten = 0;
            else
                return -1;
        }
        nleft  -= nwritten;
        buffer += nwritten;
    }

    return n;
}
*/

int main() {
    int list_s;                
    int conn_s;                
    struct sockaddr_in servaddr;  
    char buffer[MAX_LINE];      
    char *endptr;                

    short int port = (short int)ECHO_PORT;


    /*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
        listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling listen()\n");
        exit(EXIT_FAILURE);
    }


    /*  Enter an infinite loop to respond
        to client requests and echo input  */

    while (1) {

        if ((conn_s = accept(list_s, NULL, NULL)) < 0) {
            fprintf(stderr, "ECHOSERV: Error calling accept()\n");
            exit(EXIT_FAILURE);
        }

        int length = read_line(conn_s, buffer);
        if (length == -1) {
            exit(EXIT_FAILURE);
        }

        IrcMessage msg;
        msg.hasError = 0;


        irc_parse_message(buffer, &msg);
        irc_print_message(&msg);

        /*  Close the connected socket  */

        if ( close(conn_s) < 0 ) {
            fprintf(stderr, "ECHOSERV: Error calling close()\n");
            exit(EXIT_FAILURE);
        }
    }
}

