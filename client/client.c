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
        recvBuff[n] = 0;
        if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error");
        }
        printf("\n >");
    }
    if (n < 0) {
        printf("\n Read Error \n");
    }
    pthread_exit(NULL);
}

int main(void) {
    int sockfd = 0, n = 0;
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6969);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int conn =
        connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (conn < 0) {
        printf("\n Error(%d) : Connect Failed \n", conn);
        return 1;
    }

    pthread_t newThread;
    pthread_create(&newThread, NULL, listenForMsg, (void *)sockfd);

    printf("> ");
    fgets(typedMsg, BUFFER_SIZE, stdin);
    while (strcmp(typedMsg, "quit\n") != 0) {
        send(sockfd, typedMsg, strlen(typedMsg) - 1, 0);
        printf("> ");
        fgets(typedMsg, BUFFER_SIZE, stdin);
    }
    close(sockfd);
    pthread_exit(NULL);

    return 0;
}
