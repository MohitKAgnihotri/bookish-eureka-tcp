#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
#include <unistd.h>


#define HOME_PAGE "HTTP/1.1 200 File Found\r\nContent-Length: 125\r\nConnection: \
                   close\r\n\r\n<HTML><HEAD><TITLE>File \
                   Found</TITLE></HEAD><BODY><h2>FILE Found</h2><hr><p>Your requested FILE \
                   was found.</p></BODY></HTML>"

#define ERROR_PAGE "HTTP/1.1 404 File Not Found\r\nContent-Length: \
                   215\r\nConnection: close\r\n\r\n<HTML><HEAD><TITLE>File No \
                   Found</TITLE></HEAD><BODY><h2>FILE Not Found</h2><hr><p>Your requested \
                   FILE was not found.</p></BODY></HTML>"

static const int MAXPENDING = 5;

int main(int argc, char *argv[]) {

    char cmd[16] = {0};
    char path[64] = {0};
    char vers[16] = {0};

    char sendbuffer[BUFSIZE], recvbuffer[BUFSIZE];
    int numBytes = 0;

    if (argc != 2)
        DieWithUserMessage("Parameter(s)", "<Server Port>");

    in_port_t servPort = atoi(argv[1]);

    int servSock;
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithSystemMessage("socket() failed");


    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);


    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("bind() failed");


    if (listen(servSock, MAXPENDING) < 0)
        DieWithSystemMessage("listen() failed");

    for (;;) {

        int clntSock = accept(servSock, (struct sockaddr *) NULL, NULL);
        if (clntSock < 0)
            DieWithSystemMessage("accept() failed");

        while ((numBytes = recv(clntSock, recvbuffer, BUFSIZE - 1, 0)) > 0) {
            recvbuffer[numBytes] = '\0';
            fputs(recvbuffer, stdout);

            if (strstr(recvbuffer, "\r\n") > 0)
                break;
        }

        if (numBytes < 0)
            DieWithSystemMessage("recv() failed");

        /* Extract HTTP components from the incoming buffer (suggest using sscanf() ) and
        store in the three arrays previously set-up */

        sscanf(recvbuffer, "%s %s %s", cmd, path, vers);

        if (strncmp(cmd, "GET", 3) == 0) {
            if (strncmp(path, "/index",6) == 0) {
                strcpy(sendbuffer, HOME_PAGE);
            } else {
                strcpy(sendbuffer, ERROR_PAGE);
            }
        } else {
            strcpy(sendbuffer, ERROR_PAGE);
        }


        //snprintf(sendbuffer, sizeof(sendbuffer), "%s", recvbuffer);
        ssize_t numBytesSent = send(clntSock, sendbuffer, strlen(sendbuffer), 0);
        if (numBytesSent < 0)
            DieWithSystemMessage("send() failed");

        close(clntSock);

    }
}
