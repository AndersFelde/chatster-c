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

typedef struct Client {
    int connfd;
    char username[15];
    int *roomClients;
    int roomId;
} Client;

typedef struct Lobby {
    int id;
    int amountClients;
    Client clients[5];
    struct Lobby *nextLobby;
} Lobby;

char typedMsg[BUFFER_SIZE];
char recvBuff[1024];
char writeBuff[1024];
Lobby *lobbies = {0};

void sendMsg(Client *client) {
    strncpy(writeBuff, recvBuff, sizeof(writeBuff));
    printf("Free space (write): %lu", sizeof(writeBuff) - strlen(writeBuff));
    write(client->connfd, writeBuff, strlen(writeBuff) + 1);
}

int addClientToLobby(Client *client, Lobby *lobby) {
    if (lobby->amountClients == 5) {
        return 0;
    }
    lobby->clients[lobby->amountClients] = *client;
    lobby->amountClients += 1;
    // BUG: Kanskje bug med counter?
    printf("%d, %d, %p, %p\n", lobby->id, lobby->amountClients, lobby->clients,
           lobby->nextLobby);
    return 1;
}

void createLobby(Client *client, Lobby *prevLobby) {
    Lobby *lobby = (Lobby *)malloc(sizeof(Lobby));
    printf("\nCreating lobby: %d\n", client->roomId);
    lobby->id = client->roomId;
    lobby->nextLobby = 0;
    lobby->amountClients = 0;
    addClientToLobby(client, lobby);

    prevLobby->nextLobby = lobby;
}

int joinRoom(Client *client) {
    Lobby *lobby = lobbies;

    while (lobby->nextLobby != 0) {
        if (lobby->id == client->roomId) {
            return addClientToLobby(client, lobby);
        }
        lobby = lobby->nextLobby;
    }
    createLobby(client, lobby);
    return 1;
}

int readLine(int *connfd) {
    // NOTE:Kopierer rett til recvBuff
    int n = 0;

    n = read(*connfd, recvBuff, sizeof(recvBuff) - 1);
    if (n > 0) {
        recvBuff[n] = '\0';
    }
    return n;
}

void *handleClient(void *_connfd) {
    int connfd = (int)_connfd;
    Client client = {connfd};

    int n = 0;

    readLine(&connfd);
    puts(recvBuff);
    strncpy(client.username, recvBuff, sizeof(client.username));

    readLine(&connfd);
    puts(recvBuff);
    client.roomId = strtol(recvBuff, NULL, 10);
    printf("%d", client.roomId);

    if (!joinRoom(&client)) {
        close(connfd);
        printf("%s failed to connet to: %d\n", client.username, client.roomId);
        pthread_exit(NULL);
    }

    printf("%s joined room: %d\n", client.username, client.roomId);

    strncpy(writeBuff, "Welcome ", sizeof(writeBuff));
    strncat(writeBuff, client.username,
            sizeof(writeBuff) - strlen(writeBuff) - 1);

    write(connfd, writeBuff, strlen(writeBuff) + 1);

    while (readLine(&connfd) > 0) {
        // printf("%s", recvBuff);
        if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error");
        }
        sendMsg(&client);
        printf("\n");
        printf("Free space (recv): %lu", sizeof(recvBuff) - strlen(recvBuff));
    }
    if (n < 0) {
        printf("\n Read Error \n");
    }
    close(connfd);
    printf("%s left room: %d\n", client.username, client.roomId);
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

    int option = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option,
                   sizeof(option)) == -1) {
        perror("setsockopt for SO_REUSEADDR failed");
        exit(1);
    }

    int e = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (e < 0) {
        printf("Failed to bind\n");
        return -1;
    }

    printf("socket binded on: %d\n", serv_addr.sin_port);

    if (listen(listenfd, 10) == -1) {
        printf("Failed to listen\n");
        return -1;
    }

    Lobby initLobby = {0, 0, 0, 0};
    lobbies = &initLobby;

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
