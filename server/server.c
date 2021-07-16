#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "room.h"

#define BUFFER_SIZE 128

char *message = "Welcome";
char typedMsg[BUFFER_SIZE];
char recvBuff[1024];

void *handleClient(void *_connfd) {
    // Client *client;
    int connfd = (int)_connfd;

    // client = createClient(connfd);

    int n = 0;
    while ((n = read(connfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
        recvBuff[n] = 0;
        printf("%s", recvBuff);
        /* if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error");
        } */
        printf("\n");
    }
    if (n < 0) {
        printf("\n Read Error \n");
    }
    pthread_exit(NULL);
}

/* void *sendMsg(void *_connfd) {
    int connfd = (int)_connfd;
    // gir error pga int er ikke samme på 32-bit vs 64-bit
    printf("> ");
    fgets(typedMsg, BUFFER_SIZE, stdin);
    while (strcmp(typedMsg, "quit\n") != 0) {
        write(connfd, typedMsg, strlen(typedMsg) - 1);
        printf("> ");
        fgets(typedMsg, BUFFER_SIZE, stdin);
    }
    close(connfd);
    pthread_exit(NULL);
    // close(i);
} */

int main(void) {
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    // int numrv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(6969);

    bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("socket binded on: %d\n", serv_addr.sin_port);

    if (listen(listenfd, 10) == -1) {
        printf("Failed to listen\n");
        return -1;
    }

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)NULL,
                        NULL);  // accept awaiting request
        // strcpy(sendBuff, "Secret message from server");
        pthread_t newThreads;
        printf("New client: %lu\n", newThreads);
        // pthread_create(&newThreads, NULL, sendMsg, (void *)connfd);
        pthread_create(&newThreads, NULL, handleClient, (void *)connfd);
        // printf("Client connected\n");
        // printf("Closing\n");
        // sleep(1);
    }

    return 0;
}
