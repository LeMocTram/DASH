#ifndef SERVER_SERVERFUNCTION_H_
#define SERVER_SERVERFUNCTION_H_

#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include "arpa/inet.h"
#include "./unistd.h"
#include "../database/db.h"
#include <map>
#include <vector>
#include <bits/stdc++.h>

// namespace
using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;
using std::vector;

// define
#define PORT 12345       // PORT of the server
#define MAX_CLIENTS 20   // MAX_CLIENTS the server can listen
#define BUFFER_SIZE 1024 // BUFFER_SIZE to handle message from client

// class for server function
class ServerFunction
{
private:
        int serverSocket;
        int existingSocket[MAX_CLIENTS];
        int existingSocketCount = 0;
        sockaddr_in serverAddress;

public:
        map<string, vector<int>> onlineUserMapWithVector;
        map<int, string> onlineUserMap;
        ServerFunction()
        {
                this->setServerSocket(this->createServerSocket());
                cout << "ServerSocket ::  " << this->getServerSocket() << endl;
        }

        // function to process server socket
        int getServerSocket();
        int setServerSocket(int sockId);

        // function to process existing socket
        int getExistingSocket(int index);
        int setExistingSocket(int index, int socketId);

        // function to process existing socket count
        int getExistingSocketCount();
        int setExistingSocketCount(int number);

        // function to initialize the server
        void setNewFileDescriptor(fd_set *fd, int &maxFD);
        int setServerAddress();
        int createServerSocket(void);
        int initServerSocket();

        // function to appcept socket of new client
        void acceptNewConnection();

        // function to check if other sockets want to login, register or send message
        int checkOperationOnOtherSocket(fd_set *fd);

        // function to process request disconnection from client
        bool disconnect(int index);

        // function to process request logout from client
        bool clientLogOut(int index);
        int deleteUserFromMapWithVector(string username, int socketID);
        int removeSocketFromExistingList(int index);

        // function to process forward message
        int receiveRequest(int index);
        int receiveMessage(int sockId, char *buff);
        string *takeConversationIdAndMessage(string sender, string text);
        int sendAndSaveMessage(int sockId, string sender, string *text);

        // check if the user is online, 2 ways of checking, SockId or Username
        bool checkUserBasedSockID(int sockID);
        bool checkUserBasedUsername(string username);

        // function to process login and register
        string processLogin(int clientSockId, char *buff);
        string processRegister(int clientSockId, char *buff);

        // function to process user list in server
        int addToExistingSocket(int sockId);
        int addNewUserToMaps(int newSocket, string username);
        int printUserList();
        int printExistingSocket();

        // function to process conversation
        bool conversationListHandler(int clientSockID);
        bool createConversationHandler(int clientSockID, std::string createConvMessage);
        bool checkConversationIDHandler(int clientSockId, std::string message);

        // function to process message history
        std::string messageHistoryHandler(std::string conversationID);

        // funtion to process participants list
        std::string participantsListHandler(string conversationID);

        // function to process add and delete user in conversation
        string addNewUserToConversation(std::string conversationID, std::string addingUser, std::string addedUser);
        string deleteUserInConversation(std::string conversationID, std::string deletor, std::string deletedUser);

        // list user when create conversation
        bool listUserHandler(int clientSockID);
};

#endif // SERVER_SERVERFUNCTION_H_
