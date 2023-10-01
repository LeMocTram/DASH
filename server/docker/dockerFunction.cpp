#include "dockerFunction.h"
// getter and setter definition
int ServerFunction::getServerSocket()
{
    return this->serverSocket;
}
int ServerFunction::setServerSocket(int sockId)
{
    this->serverSocket = sockId;
    return 0;
}
int ServerFunction::getClientCount()
{
    return this->clientCount;
}
int ServerFunction::setClientCount(int clientCount)
{
    this->clientCount = clientCount;
    return 0;
}
int ServerFunction::getExistingSocket(int index)
{
    return this->existingSocket[index];
}
int ServerFunction::setExistingSocket(int index, int sockId)
{
    this->existingSocket[index] = sockId;
    return 0;
}
Client ServerFunction::getClient(int index)
{
    return this->clientArray[index];
}
int ServerFunction::setClient(int index, Client *client)

{
    this->clientArray[index] = *client;
    return 0;
}
int ServerFunction::getExistingSocketCount()
{
    return this->existingSocketCount;
}
int ServerFunction::setExistingSocketCount(int number)
{
    this->existingSocketCount = number;
    return 0;
}
/*                   Function Definition                     */
Client *ServerFunction::searchClientBasedSockID(int sockId)
{
    Client *temp = new Client;
    for (int i = 0; i < this->getClientCount(); i++)
    {
        if (this->getClient(i).socket_id == sockId)
        {
            *temp = this->getClient(i);
            return temp;
        }
    }
    return NULL;
}

void ServerFunction::setNewFileDescriptor(fd_set *fd, int &maxFD)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (this->getExistingSocket(i) > 0)
        {
            FD_SET(this->getExistingSocket(i), fd);
        }
        if (this->getExistingSocket(i) > maxFD)
        {
            maxFD = this->getExistingSocket(i);
        }
    }
}

int ServerFunction::setServerAddress()
{
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    this->serverAddress.sin_port = htons(PORT);
    // Return 0 when success/////////////////////
    return 0;
}

int ServerFunction::createServerSocket()
{
    // Create Server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Can not create Server socket
    if (serverSocket == -1)
    {
        cerr << "Failed to create server socket.\n";
    }
    return serverSocket;
}
// function createSocket for server socket
int ServerFunction::initServerSocket()
{
    int opt = 1;
    // Set socket opt
    if (setsockopt(this->getServerSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        cerr << "setsockopt";
        exit(EXIT_FAILURE);
    }
    // Bind socket
    if (bind(this->getServerSocket(), (struct sockaddr *)&this->serverAddress, sizeof(this->serverAddress)) == -1)
    {
        cerr << "Binding failed.\n";
        exit(EXIT_FAILURE);
    }
    // Listen for client
    if (listen(this->getServerSocket(), MAX_CLIENTS) == -1) // MAX_CLIENTS is the maximum number of clients the server can handle
    {
        cerr << "Listening failed.\n";
        exit(EXIT_FAILURE);
    }
    cout << "Server is listening...\n";
    return 0; // return 0 when success
}
// Create socket for client
void ServerFunction::acceptNewConnection()
{
    struct sockaddr_in newAddr;
    // Size of newAddr
    socklen_t newAddrSize = sizeof(newAddr);
    // Create Client socket
    int newSocket = accept(this->getServerSocket(), (struct sockaddr *)&newAddr, &newAddrSize);
    // Can not create Client socket
    if (newSocket == -1)
    {
        cerr << "Accepting failed.\n";
        // exit(1);
    }
    // Convert clientIp from 0.0.0.0 to string format
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &newAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    cout << "--------- New Connection has been accepted -------\n";
    // Output clientIp in string format
    cout << "IP Address: " << clientIP << ", Port: " << ntohs(newAddr.sin_port) << ", SockID: " << newSocket << endl;
    // add this new connection to existingSocket list
    this->addToExistingSocket(newSocket);
    this->printExistingSocket();
}
// function to check if other sockets want to login, register or send message
int ServerFunction::checkOperationOnOtherSocket(fd_set *fdSocketSet)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (FD_ISSET(this->getExistingSocket(i), fdSocketSet))
        {
            int messageLen = this->receiveRequest(i);
            if (messageLen <= 0)
            {
                this->disconnect(i);
            }
        }
    }
    return 0;
}
// function to receive disconnection from client
bool ServerFunction::disconnect(int index)
{
    // check if this is an online client or unverified connection
    Client *client;
    client = this->searchClientBasedSockID(this->getExistingSocket(index));
    if (client != NULL)
    {
        cout << "-------- \"" << client->username
             << "\" (IP: " << inet_ntoa(client->address.sin_addr)
             << ", port: " << ntohs(client->address.sin_port)
             << ") is DISCONNECTED --------" << endl;

        this->updateClientArray(client->index);
        cout << "number of remaining Users: " << this->clientCount << endl;
        this->removeSocketFromExistingList(index);
    }
    else
    {
        cout << "-------unverified connection is closed------" << endl;
        this->removeSocketFromExistingList(index);
    }
    delete (client);
    return 0;
}
// function to remove disconnected socket from existingList
int ServerFunction::removeSocketFromExistingList(int index)
{
    close(this->getExistingSocket(index));
    this->setExistingSocket(index, 0);
    this->setExistingSocketCount(this->getExistingSocketCount() - 1);
    cout << "total socket: " << this->getExistingSocketCount() << endl;
    this->printExistingSocket();
    return 0;
}
// function to remove a Client from a list of online user
int ServerFunction::updateClientArray(int index)
{
    Client temp;
    for (int i = index; i < this->getClientCount(); i++)
    {
        temp = this->getClient(i + 1);
        temp.index--;
        this->setClient(i, &temp);
    }
    temp = {"", 0, -1};
    memset(&temp.address, 0, sizeof(struct sockaddr_in));
    // decrease clientCount by 1
    this->setClientCount(this->getClientCount() - 1);
    this->setClient(this->getClientCount(), &temp);
    this->printClientList();
    return 0;
}
// function to handle request from sockets
int ServerFunction::receiveRequest(int index)
{
    // Set buffer for clientMessage
    char buffer[BUFFER_SIZE];
    // Clear buffer
    memset(buffer, 0, sizeof(buffer));
    // If bytesRead <= 0, end connection
    int bytesRead = recv(this->getExistingSocket(index), buffer, sizeof(buffer), 0);
    // The server has received the message
    if (bytesRead > 0)
    {
        if (buffer[0] == 'L')
        {
            string username = this->processLogin(this->getExistingSocket(index), buffer);
            if (username != "")
                this->addNewClient(this->getExistingSocket(index), username);
        }
        else if (buffer[0] == 'R')
        {
            string username = this->processRegister(this->getExistingSocket(index), buffer);
            if (username != "")
                this->addNewClient(this->getExistingSocket(index), username);
        }
        else
        {
            forwardMessage(this->getExistingSocket(index), buffer);
        }
    }
    // Return client Message
    return bytesRead;
}
// function to handle message
int ServerFunction::forwardMessage(int sockId, char *buff)
{
    string text(buff);
    Client *sender;
    sender = this->searchClientBasedSockID(sockId);
    if (sender == NULL)
    {
        send(sockId, "cannot send messge", sizeof("cannot send messge"), 0);
        return 1;
    }
    text = text.substr(1);
    text = '[' + sender->username + ']' + " >> " + text;
    cout << text << endl;
    for (int i = 0; i < this->getClientCount(); i++)
    {
        if (this->getClient(i).socket_id == sockId)

            continue;
        send(this->getClient(i).socket_id, text.c_str(), BUFFER_SIZE, 0);
    }
    delete (sender);
    return 0;
}
string ServerFunction::processLogin(int clientSockId, char *buff)
{
    bool isLoggedIn;
    string text(buff);
    string response;
    // cout << text << endl;
    string username, password;
    size_t pos;

    pos = text.find("$");
    username = text.substr(1, pos - 1);
    password = text.substr(pos + 1);
    cout << "----- Login request ----- " << endl;

    cout << "USERNAME: \"" << username << "\" "
         << "PASSWORD: \"" << password << "\"" << endl;
    Database db;
    db.connectMySQLHandler();
    isLoggedIn = db.authenticateLogin(username, password);
    response = "yes";
    if (isLoggedIn == false)
    {
        response = "no";
        username = "";
        cout << "Log-in Denied" << endl;
    }
    else
        cout << "Log-in Succesfully " << endl;
    send(clientSockId, response.c_str(), sizeof(response), 0);

    return username;
}
string ServerFunction::processRegister(int clientSockId, char *buff)
{
    bool isRegisted;
    string response;
    string text(buff);
    cout << text << endl;
    string username, password, email, fullName;
    size_t first, second, third;

    first = text.find('$');
    second = text.find('$', first + 1);
    third = text.find('$', second + 1);

    username = text.substr(1, first - 1);
    password = text.substr(first + 1, second - (first + 1));
    email = text.substr(second + 1, third - (second + 1));
    fullName = text.substr(third + 1);

    cout << "----- Register Request ----- " << endl;
    cout << "USERNAME: \"" << username << "\" "
         << "PASSWORD: \"" << password << "\" "
         << "EMAIL: \"" << email << "\" "
         << "FULLNAME: \"" << fullName << "\"" << endl;

    // cout << "MYSQL CHECK";
    Database db;
    // cout << "MYSQL CHECK";
    db.connectMySQLHandler();
    cout << "MYSQL ERROR";
    isRegisted = db.insertRegUser(username, password, email, fullName);
    response = "yes";
    if (isRegisted == false)
    {
        username = "";
        response = "no";
        cout << "Register Denied" << endl;
    }
    else
        cout << "Register Succesfully " << endl;
    send(clientSockId, response.c_str(), sizeof(response), 0);
    return username;
}
// add new connection to existingSocket list
int ServerFunction::addToExistingSocket(int newSocket)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (this->getExistingSocket(i) == 0)
        {
            this->setExistingSocket(i, newSocket);
            this->setExistingSocketCount(this->getExistingSocketCount() + 1);
            cout << "----added to existing socket list------" << endl;
            cout << "total socket: " << this->getExistingSocketCount() << endl;
            return 0;
        }
    }
    return 1;
}
// function to add an existing socket to list of Online User
int ServerFunction::addNewClient(int newSocket, string username)
{
    Client newClient;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        // if position is empty
        if (this->getClient(i).socket_id == 0)
        {
            newClient = {username, newSocket, i};
            getpeername(newSocket, (struct sockaddr *)&newClient.address,
                        reinterpret_cast<socklen_t *>(&newClient.add_len));
            this->setClient(i, &newClient);
            this->setClientCount(this->getClientCount() + 1);

            cout << "------ \"" << this->getClient(i).username << "\" ("
                 << inet_ntoa(this->getClient(i).address.sin_addr)
                 << ", " << ntohs(this->getClient(i).address.sin_port)
                 << ") is Added to list of Online User ----- "
                 << this->getClient(i).index << endl;
            cout << "number of online User: " << this->getClientCount() << endl;
            this->printClientList();
            break;
        }
    }
    return 0;
}
int ServerFunction::printClientList()
{
    cout << "------ List of Client -----" << endl;
    cout << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << '|' << std::setw(15) << "USERNAME" << std::setw(15)
         << '|' << std::setw(15) << "SOCK ID" << std::setw(15) << '|' << endl;
    for (int i = 0; i < this->getClientCount(); i++)
    {
        cout << "+-----------------------------------------------------------+" << endl;
        cout << '|' << std::setw(15) << this->getClient(i).username << std::setw(15)
             << '|' << std::setw(15) << this->getClient(i).socket_id << std::setw(15)
             << '|' << this->getClient(i).index << endl;
    }
    cout << "+-----------------------------------------------------------+" << endl;
    return 0;
}
int ServerFunction::printExistingSocket()
{
    cout << "----- List of Socket -----" << endl;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        cout << this->getExistingSocket(i) << " ";
    }
    cout << endl;
    return 0;
}