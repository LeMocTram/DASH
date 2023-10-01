#ifndef DATABASE_DB_H_
#define DATABASE_DB_H_
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "mysql/mysql.h"
#include "./string.h"
#include <bits/stdc++.h>

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

    // function to setup environment for database
    bool connectDatabase();

    // function to connect database with server
    bool connectMySQLHandler();

    std::string queryMySQLHandler(std::string sqlQueryCommand);

    // funtion to process insert new user in user table
    bool insertRegUser(std::string clientUsername, std::string clientPassword, std::string emailClient, std::string fullnameClient);

    // Query password, check if it user is existed or not
    std::string queryUserHandler(std::string clientUsername);

    // function to send conversation details to server
    std::string sendConversationDetails(struct Conversation conversation[], int arrLength);

    // function to query conversation and send conversation to server
    std::string queryConversation(std::string username);

    // function to query message history
    std::string queryMessageHandler(std::string conversationID);

    // function to authenticate user when user login
    bool authenticateLogin(std::string clientUsername, std::string clientPassword);

    // function to insert new conversation, return conversationID of new conversation insert
    std::string insertConversation(std::string conversationName);

    // function to get number of participants
    int queryNumberOfParticipants(std::string conversationID);

    // function to get conversation name by conversation
    std::string queryConversationName(std::string conversationID);

    // function to get member list in conversation
    void queryConversationParticipants(std::string conversationID, std::vector<std::string> &participants);

    // function to get conversation detail
    int queryConversationDetails(std::string conversationID, Conversation conversation[], int curPosition);

    // function to insert user in participate table
    bool insertParticipate(std::string conversationID, std::string username);

    // function to check username and password is exist in PARTICIPATE
    bool checkUsernameIsExistInParticipate(std::string conversationID, std::string username);

    // function insert new conversation
    std::string insertConversationHandler(std::string conversationName, std::string userCreator);

    // function to insert new message and messageID auto increment
    bool insertMessageHandler(std::string username, std::string conversationID, std::string message);

    // function to check private chat exist or not
    bool checkPrivateChatIsExist(std::string username1, std::string username2);

    // function to check list user have a common group or not
    bool checkPublicChatIsExist(std::string numberOfUser, std::string usernameList, std::string conversationName);

    // Insert administrate
    bool insertAdministrate(std::string conversationID, std::string username);

    // function to delete user from participate
    bool deleteUserInParticipate(std::string conversationID, std::string username);

    // function to check user have admin rights or not
    bool queryAdministrateHandler(std::string conversationID, std::string username);

    // function to list user when create conversation
    std::string queryListUserHandler();

    // function to close connection with server
    void closeConnection()
    {
        mysql_close(this->connection);
    }
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
