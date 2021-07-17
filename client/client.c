#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 128

char typedMsg[BUFFER_SIZE];
char recvBuff[1024];

void *listenForMsg(void *_sockfd) {
    int n = 0;
    int sockfd = (int)_sockfd;
    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
        recvBuff[n] = 0x00;
        // strncat(recvBuff, "\nok> ", sizeof(recvBuff) - strlen(recvBuff));
        if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error");
        }
    }
    if (n < 0) {
        printf("\n Read Error \n");
    }
    pthread_exit(NULL);
}

int main(void) {
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6969);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Connecting on: %d\n", serv_addr.sin_port);
    int conn =
        connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (conn < 0) {
        printf("\n Error(%d) : Connect Failed \n", conn);
        return 1;
    }

    pthread_t listenThread;
    pthread_create(&listenThread, NULL, listenForMsg, (void *)sockfd);

    printf("Username: ");
    /* fgets(typedMsg, BUFFER_SIZE, stdin);  // username
    send(sockfd, typedMsg, strlen(typedMsg) - 1, 0);
    printf("RoomId: "); */
    while (strcmp(typedMsg, "quit\n") != 0) {
        send(sockfd, typedMsg, strlen(typedMsg) - 1, 0);
        printf("> ");
        fgets(typedMsg, BUFFER_SIZE, stdin);
    }
    close(sockfd);
    printf("Closed socket");
    pthread_cancel(listenThread);
    pthread_exit(NULL);

    return 0;
}
