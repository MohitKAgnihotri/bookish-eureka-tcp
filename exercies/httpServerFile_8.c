#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
#include <unistd.h>
#include <sys/stat.h>


static const int MAXPENDING = 5;

int main(int argc, char *argv[]) {

    char cmd[16] = {0};
    char path[64] = {0};
    char vers[16] = {0};

    char discard1[2048] = {0};
    char discard2[2048] = {0};

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

    // Configure server socket (makes debugging easy)
    int enable = 1;
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

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
            if (strncmp(path, "/index",6) == 0 || strncmp(path, "/",1) == 0) {
                // Try to open the file and send it to the client

                // Try to open the file (if it exist)
                FILE *fp = fopen("index.html", "r");
                if (fp == NULL) {
                    DieWithSystemMessage("fopen() failed");
                }

                //get file size using stat
                struct stat st;
                stat("index.html", &st);
                int size = st.st_size;

                // Send HTTP response 200 OK with file contents
                sprintf(sendbuffer, "HTTP/1.1 200 OK\r");
                strcat(sendbuffer, "Content-Type: text/html\r");
                strcat(sendbuffer, "Content-Length: ");
                sprintf(discard1, "%d", size);
                strcat(sendbuffer, discard1);
                // Add file contents
                fread(discard2, 1, size, fp);
                strcat(sendbuffer, discard2);
                strcat(sendbuffer, "\r\r");
                // Try to open the file and send it to the client
            } else {
                // Send a 404 error to the client with contents from a file

                // Try to open the file

                // Try to open the file (if it exist)
                FILE *fp = fopen("error.html", "r");
                if (fp == NULL) {
                    DieWithSystemMessage("fopen() failed");
                }

                //get file size using stat
                struct stat st;
                stat("error.html", &st);
                int size = st.st_size;

                // Send HTTP response 200 OK with file contents

                sprintf(sendbuffer, "HTTP/1.1 404 Not Found\r");
                strcat(sendbuffer, "Content-Type: text/html\r");
                strcat(sendbuffer, "Content-Length: ");
                sprintf(discard1, "%d", size);
                strcat(sendbuffer, discard1);
                // Add file contents
                fread(discard2, 1, size, fp);
                strcat(sendbuffer, discard2);
                strcat(sendbuffer, "\r\r");


            }
        } else {
            // Send a 404 error to the client with contents from a file

            // Try to open the file

            // Try to open the file (if it exist)
            FILE *fp = fopen("error.html", "r");
            if (fp == NULL) {
                DieWithSystemMessage("fopen() failed");
            }

            //get file size using stat
            struct stat st;
            stat("error.html", &st);
            int size = st.st_size;

            // Send HTTP response 200 OK with file contents

            sprintf(sendbuffer, "HTTP/1.1 404 Not Found\r");
            strcat(sendbuffer, "Content-Type: text/html\r");
            strcat(sendbuffer, "Content-Length: ");
            sprintf(discard1, "%d", size);
            strcat(sendbuffer, discard1);
            // Add file contents
            fread(discard2, 1, size, fp);
            strcat(sendbuffer, discard2);
            strcat(sendbuffer, "\r\r");
        }


        //snprintf(sendbuffer, sizeof(sendbuffer), "%s", recvbuffer);
        ssize_t numBytesSent = send(clntSock, sendbuffer, strlen(sendbuffer), 0);
        if (numBytesSent < 0)
            DieWithSystemMessage("send() failed");

        close(clntSock);

    }
}
