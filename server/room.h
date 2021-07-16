#ifndef room
#define room
typedef struct Client {
    int connfd;
    char username[15];
    int *roomClients;
} Client;

Client *createClient(int connfd);
int sendMsg(Client client);
int joinRoom(Client client);

#endif
