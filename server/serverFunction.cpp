#include "serverFunction.h"

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
int ServerFunction::getExistingSocket(int index)
{
    return this->existingSocket[index];
}
int ServerFunction::setExistingSocket(int index, int sockId)
{
    this->existingSocket[index] = sockId;
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

// function to set for new file descriptor
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

// function to set server adddress
int ServerFunction::setServerAddress()
{
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    this->serverAddress.sin_port = htons(PORT);
    // Return 0 when success
    return 0;
}

// function to create socket for server
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

// funtion to init server socket: set socket opt, bind socket and listen client
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

// create socket for client
void ServerFunction::acceptNewConnection()
{
    struct sockaddr_in newAddr;
    // Size of newAddr
    socklen_t newAddrSize = sizeof(newAddr);
    // Create client socket
    int newSocket = accept(this->getServerSocket(), (struct sockaddr *)&newAddr, &newAddrSize);
    // Can not create client socket
    if (newSocket == -1)
    {
        cerr << "Accepting failed.\n";
    }
    // Convert clientIP from 0.0.0.0 to string format
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &newAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    cout << "--------- New Connection has been accepted -------\n";
    // Output clientIP in string format
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
    int socketID = this->getExistingSocket(index);
    if (this->checkUserBasedSockID(socketID))
    {
        string username = this->onlineUserMap[socketID];
        cout << "-------- \"" << username << "\"   is DISCONNECTED --------" << endl;
        // delete from the MapWitVector
        this->deleteUserFromMapWithVector(username, socketID);
        this->onlineUserMap.erase(socketID);
        this->printUserList();
        cout << "number of remaining Users: " << this->onlineUserMapWithVector.size() << endl;
        this->removeSocketFromExistingList(index);
    }
    else
    {
        cout << "-------unverified connection is closed------" << endl;
        this->removeSocketFromExistingList(index);
    }
    return 0;
}

// function to process request logout from client
bool ServerFunction::clientLogOut(int socketID)
{
    cout << "sockID of log out client: " << socketID << endl;
    string username = this->onlineUserMap[socketID];
    cout << "-------- \"" << username << "\"   is LOGGED OUT --------" << endl;
    // delete from the MapWithVector
    this->deleteUserFromMapWithVector(username, socketID);
    this->onlineUserMap.erase(socketID);
    this->printUserList();
    cout << "number of remaining Users: " << this->onlineUserMapWithVector.size() << endl;
    return 0;
}

// function to delete user from map
int ServerFunction::deleteUserFromMapWithVector(string username, int socketID)
{
    // check if this user is still logged-in on other devices, if it is, just remove
    // the socketID from the vector
    if (this->onlineUserMapWithVector[username].size() > 1)
    {
        for (auto i = this->onlineUserMapWithVector[username].begin();
             i != this->onlineUserMapWithVector[username].end(); ++i)
        {
            if (*i == socketID)
            {
                this->onlineUserMapWithVector[username].erase(i);
                break;
            }
        }
    }
    // if it is not, erase this User from the Map
    else
    {
        this->onlineUserMapWithVector.erase(username);
    }
    cout << "-----------\"" << username << "\" is deleted from MapWithVector ----------" << endl;
    return 0;
}

// function to remove disconnected socket from existing list
int ServerFunction::removeSocketFromExistingList(int index)
{
    close(this->getExistingSocket(index));
    this->setExistingSocket(index, 0);
    this->setExistingSocketCount(this->getExistingSocketCount() - 1);
    cout << "total socket: " << this->getExistingSocketCount() << endl;
    this->printExistingSocket();
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
        if (buffer[0] == 'L' && buffer[1] != 'U')
        {
            string username = this->processLogin(this->getExistingSocket(index), buffer);
            if (username != "")
            {
                this->addNewUserToMaps(this->getExistingSocket(index), username);
            }
        }
        else if (buffer[0] == 'R')
        {
            string username = this->processRegister(this->getExistingSocket(index), buffer);
            if (username != "")
                this->addNewUserToMaps(this->getExistingSocket(index), username);
        }
        else if (this->checkUserBasedSockID(this->getExistingSocket(index)))
        {
            if (buffer[0] == 'C' && buffer[1] == 'C')
            {
                this->createConversationHandler(this->getExistingSocket(index), std::string(buffer));
            }
            else if (buffer[0] == 'Q' && buffer[1] == 'C')
            {
                this->conversationListHandler(this->getExistingSocket(index));
            }
            else if (buffer[0] == 'A' && buffer[1] == 'N')
            {
                // add user to conversation string
                // str = "AN$conversationID$adder$addedUser"
                int secondSignPos = string(buffer).find("$", 3);
                int thirdSignPos = string(buffer).find("$", secondSignPos + 1);
                string conversationID = string(buffer).substr(3, secondSignPos - 3);
                string addingUser = string(buffer).substr(secondSignPos + 1, thirdSignPos - secondSignPos - 1);
                string addedUser = string(buffer).substr(thirdSignPos + 1);
                string message = this->addNewUserToConversation(conversationID, addingUser, addedUser);
                message = "A" + message;
                send(this->getExistingSocket(index), message.c_str(), message.length(), 0);
            }
            else if (buffer[0] == 'D' && buffer[1] == 'E')
            {
                // delete user from conversation string
                // str = "DE$conversationID$deletor$deletedUser"
                int secondSignPos = string(buffer).find("$", 3);
                int thirdSignPos = string(buffer).find("$", secondSignPos + 1);
                string conversationID = string(buffer).substr(3, secondSignPos - 3);
                string deletor = string(buffer).substr(secondSignPos + 1, thirdSignPos - secondSignPos - 1);
                string deletedUser = string(buffer).substr(thirdSignPos + 1);
                string message = this->deleteUserInConversation(conversationID, deletor, deletedUser);
                message = "D" + message;
                send(this->getExistingSocket(index), message.c_str(), message.length(), 0);
            }
            else if (buffer[0] == 'Q' && buffer[1] == 'P')
            {
                string conversationID = string(buffer).substr(3, string(buffer).find("$", 3) - 3);
                std::string message = this->participantsListHandler(conversationID);
                send(this->getExistingSocket(index), message.c_str(), message.length(), 0);
            }
            else if (buffer[0] == 'I' && buffer[1] == 'D')
            {
                this->checkConversationIDHandler(this->getExistingSocket(index), std::string(buffer));
            }
            else if (buffer[0] == 'O')
            {
                cout << "log-out request!!!!" << endl;
                this->clientLogOut(this->getExistingSocket(index));
            }
            else if (buffer[0] == 'L' && buffer[1] == 'U')
            {
                this->listUserHandler(this->getExistingSocket(index));
            }
            else
            {
                // format of buffer[]:  M$conversationID$message_content
                receiveMessage(this->getExistingSocket(index), buffer);
            }
        }
    }
    return bytesRead;
}

// function to receive message and check if the sender is a valid user
int ServerFunction::receiveMessage(int sockId, char *buff)
{
    string text(buff);
    string sender = "";
    // check if this sender is an unverified user
    if (!(this->checkUserBasedSockID(sockId)))
    {
        cout << "NULL sender " << endl;
        send(sockId, "cannot send messge", sizeof("cannot send messge"), 0);
        return 1;
    }
    // find the sender of this message
    sender = this->onlineUserMap[sockId];
    cout << "sender sockID: " << sockId << endl;
    // handling the received buffer to take converssationID and message content
    string *result = this->takeConversationIdAndMessage(sender, text);
    // after getting conversationID and message_content, start forwaring to onther users
    // and save message to database
    this->sendAndSaveMessage(sockId, sender, result);
    delete[] result;
    return 0;
}

// take neccessary informations from the buffer received from sender
string *ServerFunction::takeConversationIdAndMessage(string sender, string text)
{
    // format of text:  M$conversationID$message_content
    string *output = new string[3];
    string conversationID, message, textSend;
    size_t firstPosition, secondPosition;

    message = text.substr(1);
    // text is nothing than a line to print on server terminal for debugging
    text = '[' + sender + ']' + " >> " + message;
    cout << text << endl;
    // textSend is the text will be forwarded to other Users
    textSend = '[' + sender + ']' + ": " + message;

    firstPosition = text.find("$");
    secondPosition = text.find("$", firstPosition + 1);
    conversationID = text.substr(firstPosition + 1, secondPosition - firstPosition - 1);
    message = text.substr(secondPosition + 1);
    // store information into array and return
    output[0] = conversationID;
    output[1] = message;
    output[2] = textSend;

    return output;
}

// forward the message to other clients and save message to database
int ServerFunction::sendAndSaveMessage(int sockId, string sender, string *text)
{
    string conversationID, message, textSend;
    vector<string> participants;
    Database db;
    db.connectMySQLHandler();

    conversationID = *(text + 0);
    message = *(text + 1);
    textSend = *(text + 2);

    db.queryConversationParticipants(conversationID, participants);
    for (auto participant = participants.begin(); participant != participants.end(); participant++)
    {
        // search if this participant is in the Map of online User
        // send message if the participant is online
        if (this->checkUserBasedUsername(*participant))
        {
            vector<int> ReceiverSockId = this->onlineUserMapWithVector[*participant];
            for (auto i = ReceiverSockId.begin(); i != ReceiverSockId.end(); ++i)
            {
                if (*participant == sender && *i == sockId)
                {
                    db.insertMessageHandler(*participant, conversationID, message);
                    continue;
                }
                send(*i, textSend.c_str(), BUFFER_SIZE, 0);
            }
        }
    }
    db.closeConnection();
    return 0;
}

// 2 functions to check if a user is online or not
bool ServerFunction::checkUserBasedSockID(int sockID)
{
    auto search = this->onlineUserMap.find(sockID);
    if (search != this->onlineUserMap.end())
    {
        return true;
    }
    return false;
}

// function to check username exist or not in online list
bool ServerFunction::checkUserBasedUsername(string username)
{
    auto search = this->onlineUserMapWithVector.find(username);
    if (search != this->onlineUserMapWithVector.end())
    {
        return true;
    }
    return false;
}

// function to process login from client
string ServerFunction::processLogin(int clientSockId, char *buff)
{
    bool isLoggedIn;
    string text(buff);
    string response;
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
    send(clientSockId, response.c_str(), sizeof(response), 0);
    db.closeConnection();

    return username;
}

// function to check user exist inparticipate when client enter conversation
// to get message from conversation
bool ServerFunction::checkConversationIDHandler(int clientSockId, std::string message)
{
    std::string conversationID, conversationName, respone, messageHistory;
    conversationID = message.substr(2);
    string sender = "";
    auto search = this->onlineUserMap.find(clientSockId);
    if (search != this->onlineUserMap.end())
        sender = search->second;

    Database db;
    db.connectMySQLHandler();
    bool checkUserInParticipate = db.checkUsernameIsExistInParticipate(conversationID, sender);
    // if user exist in conversation, return message history
    if (checkUserInParticipate == true)
    {
        respone = "yes";
        messageHistory = this->messageHistoryHandler(conversationID);
        send(clientSockId, messageHistory.c_str(), messageHistory.length(), 0);
    }
    else
    {
        respone = "no";
        send(clientSockId, respone.c_str(), respone.length(), 0);
    }
    db.closeConnection();
    return 0;
}

// function to process register new user
string ServerFunction::processRegister(int clientSockId, char *buff)
{
    bool isRegisted;
    string response;
    string text(buff);
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

    Database db;
    db.connectMySQLHandler();
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
    db.closeConnection();
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
int ServerFunction::addNewUserToMaps(int newSocket, string username)
{
    //  check if this new client is already logged_in on other devices
    //  if it is, add this socket to the vector of socket
    if (this->checkUserBasedUsername(username))
    {
        this->onlineUserMapWithVector[username].push_back(newSocket);
        cout << "-----this socketID has been added to already connected User----- " << endl;
        this->onlineUserMap.insert({newSocket, username});
        cout << "-----Added to OnlineUserMap----- " << endl;
    }
    // if it isn't, add it as a completely new online User
    else
    {
        this->onlineUserMapWithVector.insert(pair<string, vector<int>>(username, {newSocket}));
        cout << "-----Added to Online USer Map With Vector----- " << endl;
        this->onlineUserMap.insert({newSocket, username});
        cout << "-----Added to OnlineUserMap----- " << endl;
    }
    this->printUserList();
    cout << "----- total Online Users: " << this->onlineUserMapWithVector.size() << " ----------" << endl;

    return 0;
}

// function to print list user in online map user
int ServerFunction::printUserList()
{
    cout << "------ List of user -----" << endl;
    cout << endl;
    for (auto itr = this->onlineUserMapWithVector.begin();
         itr != this->onlineUserMapWithVector.end(); ++itr)
    {
        cout << itr->first << " ( ";
        for (auto c : itr->second)
        {
            cout << c << " ";
        }
        cout << ")" << endl;
    }
    cout << endl;
    cout << "END LIST" << endl;
    return 0;
}

// function to print existing socket list
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

// function to create new conversation
bool ServerFunction::createConversationHandler(int clientSockID, std::string createConvMessage)
{
    string username = "";
    auto search = this->onlineUserMap.find(clientSockID);
    if (search != this->onlineUserMap.end())
        username = search->second;
    Database db;
    db.connectMySQLHandler();
    // string str ="CC$NAME$4$thinh$khanh$trung$hung$"
    size_t first, second, third;
    std::string convName;

    // sub string to get number of participants and user list
    first = createConvMessage.find('$');
    second = createConvMessage.find('$', first + 1);
    third = createConvMessage.find('$', second + 1);

    convName = createConvMessage.substr(first + 1, second - (first + 1));
    int numOfParticipants = stoi(createConvMessage.substr(second + 1, third - (second + 1)));

    string *list = new string[numOfParticipants];
    int i = 0;
    int secondPointer = third + 1, firstPointer = third;

    while (i < numOfParticipants)
    {
        while (createConvMessage[secondPointer] != '$')
            secondPointer++;
        list[i] = createConvMessage.substr(firstPointer + 1, secondPointer - (firstPointer + 1));
        i++;
        firstPointer = secondPointer;
        secondPointer++;
    }

    // Check user exist or not
    std::string respone;
    for (int i = 0; i < numOfParticipants; i++)
    {
        if (db.queryUserHandler(*(list + i)) == "")
        {
            respone = "no";
            send(clientSockID, respone.c_str(), respone.length(), 0);
            return false;
        }
    }

    // Check group or private chat exist or in database
    if (numOfParticipants > 1)
    {
        std::string listUserInGroup = "";
        listUserInGroup += "\"" + username + "\"";
        for (int i = 0; i < numOfParticipants; i++)
        {
            listUserInGroup += ", \"" + *(list + i) + "\"";
        }
        if (db.checkPublicChatIsExist(std::to_string(numOfParticipants + 1), listUserInGroup, convName))
        {
            std::cout << "The group name must be different!" << std::endl;
            respone = "groupIsExist";
            send(clientSockID, respone.c_str(), respone.length(), 0);
            return false;
        }
    }
    else
    {
        std::string addedUser(*(list + 0));
        if (db.checkPrivateChatIsExist(username, addedUser))
        {
            std::cout << "The private chat is exist!" << std::endl;
            respone = "privateChatIsExist";
            send(clientSockID, respone.c_str(), respone.length(), 0);
            return false;
        }
    }

    // insert creater in conversation
    std::string newconversationID = db.insertConversationHandler(convName, username);
    if (numOfParticipants > 1)
    {
        db.insertAdministrate(newconversationID, username);
    }
    // insert user list in participate when create group chat
    for (int i = 0; i < numOfParticipants; i++)
    {
        db.insertParticipate(newconversationID, *(list + i));
    }
    std::cout << "Create conversation success" << convName << std::endl;
    respone = "yes";
    send(clientSockID, respone.c_str(), respone.length(), 0);
    db.closeConnection();
    return true;
}

// function to list conversation
bool ServerFunction::conversationListHandler(int clientSockID)
{
    string username = "";
    auto search = this->onlineUserMap.find(clientSockID);
    if (search != this->onlineUserMap.end())
        username = search->second;
    Database db;
    db.connectMySQLHandler();
    std::string message = db.queryConversation(username);
    std::string sendMessage = "";
    int startPos = 0;
    int endPos = 0;

    char queryConversationBuffer[BUFFER_SIZE];
    while ((endPos = message.find('|', startPos)) != -1)
    {
        memset(queryConversationBuffer, 0, sizeof(queryConversationBuffer));
        endPos = message.find('|', startPos);
        sendMessage = message.substr(startPos, endPos - startPos);
        send(clientSockID, sendMessage.c_str(), BUFFER_SIZE, 0);
        startPos = endPos + 1;
    }
    std::string endNotify = "End conversation";
    send(clientSockID, endNotify.c_str(), endNotify.length(), 0);
    db.closeConnection();
    return true;
}

// function to list user in participants
std::string ServerFunction::participantsListHandler(string conversationID)
{
    Database db;
    db.connectMySQLHandler();
    vector<string> participantList;
    db.queryConversationParticipants(conversationID, participantList);
    string message = "Q";
    for (auto i = participantList.begin(); i != participantList.end(); i++)
    {
        message += *i + "$";
    }
    db.closeConnection();
    return message;
}

// function to add user in group chat
string ServerFunction::addNewUserToConversation(std::string conversationID, std::string addingUser, std::string addedUser)
{
    Database db;
    db.connectMySQLHandler();
    int numberOfParticipants = db.queryNumberOfParticipants(conversationID);
    std::string addUserMesaage = "";
    bool joined = db.checkUsernameIsExistInParticipate(conversationID, addedUser);
    std::string checkUserIsExist = db.queryUserHandler(addedUser);
    // private chat not allowed add user
    if (numberOfParticipants < 3)
    {
        addUserMesaage = "\033[1m\033[33mThis is a private chat, add feature is not allowed.\033[0m";
    }
    // check admin rights to delete user
    else if (!db.queryAdministrateHandler(conversationID, addingUser))
    {
        addUserMesaage = "\033[1m\033[33mUser does not permission to add user!\033[0m";
    }
    else if (joined == true)
    {
        addUserMesaage = "\033[1m\033[33mThe user is already in the chat.\033[0m";
    }
    else if (checkUserIsExist != "")
    {
        db.insertParticipate(conversationID, addedUser);
        addUserMesaage = "\033[1m\033[32mAdd success!\033[0m";
    }
    else
    {
        addUserMesaage = "\033[1m\033[33mThe user does not exist!\033[0m";
    }
    db.closeConnection();
    return addUserMesaage;
}

// Delete user in conversation
string ServerFunction::deleteUserInConversation(std::string conversationID, std::string deletor, std::string deletedUser)
{
    Database db;
    db.connectMySQLHandler();
    std::string deleteUserMessage;
    if (db.queryNumberOfParticipants(conversationID) == 2)
    {
        deleteUserMessage = "\033[1m\033[33mThis is a private chat, delete feature is not allowed.\033[0m";
    }
    else if (db.queryNumberOfParticipants(conversationID) == 3)
    {
        deleteUserMessage = "\033[1m\033[33mThe minimum number of public chat is 3 users.\nCan not delete!\033[0m";
    }
    else if (!db.queryAdministrateHandler(conversationID, deletor))
    {
        deleteUserMessage = "\033[1m\033[33mUser does not have permission to delete!\033[0m";
    }
    else if (deletor == deletedUser)
    {
        deleteUserMessage = "\033[1m\033[33mYou are admin! Can not delete youself!\033[0m";
    }
    // Check user exist or not
    else if (db.queryUserHandler(deletedUser) == "")
    {
        deleteUserMessage = "\033[1m\033[33mUser does not exist!\033[0m";
    }
    else if (!db.checkUsernameIsExistInParticipate(conversationID, deletedUser))
    {
        deleteUserMessage = "\033[1m\033[33mUser is not in this group!\033[0m";
    }
    else
    {
        db.deleteUserInParticipate(conversationID, deletedUser);
        deleteUserMessage = "\033[1m\033[32mDelete successfully!\033[0m";
    }
    db.closeConnection();
    return deleteUserMessage;
}

// function to get message history
std::string ServerFunction::messageHistoryHandler(std::string conversationID)
{
    Database db;
    db.connectMySQLHandler();
    std::string messageHistory = db.queryMessageHandler(conversationID);
    db.closeConnection();
    return messageHistory;
}

// function to list user when create conversation
bool ServerFunction::listUserHandler(int clientSockID)
{
    Database db;
    db.connectMySQLHandler();
    std::string listUser = db.queryListUserHandler();
    send(clientSockID, listUser.c_str(), listUser.length(), 0);
    return true;
}
