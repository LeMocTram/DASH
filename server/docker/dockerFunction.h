#ifndef SERVER_SERVERFUNCTION_H_
#define SERVER_SERVERFUNCTION_H_

#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include "arpa/inet.h"
#include "./unistd.h"
#include "../../database/docker/dbDocker.h"

// namespace

using std::cerr;
using std::cout;
using std::endl;
using std::string;
// define

#define PORT 12345       // PORT of the server
#define MAX_CLIENTS 20   // MAX_CLIENTS the server can listen
#define BUFFER_SIZE 1024 // BUFFER_SIZE to handle message from client

struct Client
{
    std::string username;
    int socket_id;
    int index;
    struct sockaddr_in address;
    socklen_t add_len = sizeof(address);
    // index in the array of connected client
};

// class for server function
class ServerFunction
{
private:
    int serverSocket;
    // Clients are available to chat
    Client clientArray[MAX_CLIENTS];
    int clientCount = 0;
    // Connection to server
    int existingSocket[MAX_CLIENTS];
    int existingSocketCount = 0;
    sockaddr_in serverAddress;

public:
    explicit ServerFunction()
    {
        this->setServerSocket(this->createServerSocket());
        cout << "ServerSocket ::  " << this->getServerSocket() << endl;
    }
    int getServerSocket();
    int setServerSocket(int sockId);
    int getClientCount();
    int setClientCount(int clientCount);
    int getExistingSocket(int index);
    int setExistingSocket(int index, int socketId);
    Client getClient(int index);
    int setClient(int index, Client *client);
    int getExistingSocketCount();
    int setExistingSocketCount(int number);

    // Declare functions
    Client *searchClientBasedSockID(int sockId);

    int setServerAddress();
    int createServerSocket(void);
    int initServerSocket();

    void setNewFileDescriptor(fd_set *fd, int &maxFD);
    void acceptNewConnection();

    int checkOperationOnOtherSocket(fd_set *fd);
    bool disconnect(int index);
    int updateClientArray(int index);
    int removeSocketFromExistingList(int index);

    int receiveRequest(int index);
    int forwardMessage(int sockId, char *buff);

    string processLogin(int clientSockId, char *buff);
    string processRegister(int clientSockId, char *buff);

    int addToExistingSocket(int sockId);
    int addNewClient(int newSocket, string username);
    int printClientList();
    int printExistingSocket();
};

#endif // SERVER_SERVERFUNCTION_H_
