#ifndef DATABASE_DB_H_
#define DATABASE_DB_H_
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "mysql/mysql.h"
#include "./string.h"

class Database
{
private:
    std::string server = "localhost";
    std::string user;
    std::string password;
    std::string schema;
    MYSQL *connection = nullptr;

public:
    explicit Database(std::string user = "server", std::string password = "123456", std::string schema = "CHATSERVICE")
    {
        this->user = user;
        this->password = password;
        this->schema = schema;
    }
    bool connectDatabase();
    bool connectMySQLHandler();

    std::string queryMySQLHandler(std::string sqlQueryCommand);

    // addUser use add new user when register
    bool insertRegUser(std::string clientUsername, std::string clientPassword, std::string emailClient, std::string fullnameClient);

    // Query password, check if it user is existed or not
    std::string queryUserHandler(std::string clientUsername);

    std::string sendConversationDetails(struct Conversation conversation[], int arrLength);

    std::string queryConversation(std::string username);

    std::string queryMessageHandler(std::string conversationID);

    // check username and password
    bool authenticateLogin(std::string clientUsername, std::string clientPassword);

    // insert conversation
    std::string insertConversation(std::string conversationName);

    // getNumberOfParticipate
    int queryNumberOfPartipants(std::string conversationID);

    // get conversation Name
    std::string queryConversationName(std::string conversationID);

    void queryConversationParticipants(std::string conversationID, std::vector<std::string> &participants);

    int queryConversationDetails(std::string conversationID, Conversation conversation[], int curPosition);

    // insert participate
    bool insertParticipate(std::string conversationID, std::string username);

    // checkUsernameIsExistInParticipate
    bool checkUsernameIsExistInParticipate(std::string conversationID, std::string username);

    // handler insert conversation
    bool insertConversationHandler(std::string conversationName, std::string userCreator);

    // insert new message
    bool insertMessageHandler(std::string username, std::string conversationID, std::string message);

    // check list user in participate
    bool checkListUserInParticipate(std::string creater, std::string usernameList);
};

struct Conversation
{
    int ID;
    std::string name = "";
    int numberOfPartipants;
    std::vector<std::string> participants;
};

int convertToInteger(char *unconvertString)
{
    int i = 0;
    int result = 0;
    while (static_cast<int>(unconvertString[i]) != 0)
    {
        result = result * 10;
        result += static_cast<int>(unconvertString[i]) - 48; // row[0][i] - '0'
        i += 1;
    }
    return result;
}

#endif // DATABASE_DB_H_
