#ifndef room
#define room
typedef struct Client {
    int connfd;
    int *roomClients;
    char username[15];
} Client;

Client *createClient(int connfd);
int sendMsg(Client client);
int joinRoom(Client client);

#endif
