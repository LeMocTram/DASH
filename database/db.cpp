#include "./db.h"

// function to setup environment for database
bool Database::connectDatabase()
{
    char *env;
    env = std::getenv("MYSQL_HOST");
    if (env != NULL)
    {
        std::cout << "MYSQL_HOST: " << std::string(env) << std::endl;
        this->server = std::string(env);
    }
    return true;
}

// function to connect database with server
bool Database::connectMySQLHandler()
{
    this->connectDatabase();
    this->connection = mysql_init(NULL);
    if (!mysql_real_connect(this->connection, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->schema.c_str(), 0, NULL, 0))
    {
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    return true;
}

// function to execute query statement
// if the query is succesful return type query, else return empty string
std::string Database::queryMySQLHandler(std::string sqlQueryCommand)
{
    std::string commandType = "";
    try
    {
        // point to query command string, create a result set
        int queryState = mysql_query(this->connection, sqlQueryCommand.c_str());
        int firstSpacePos = sqlQueryCommand.find(' ');
        commandType = sqlQueryCommand.substr(0, firstSpacePos);
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return commandType;
}

// funtion to process insert new user in user table
bool Database::insertRegUser(std::string clientUsername, std::string clientPassword, std::string emailClient, std::string fullnameClient)
{
    if (this->queryUserHandler(clientUsername) != "")
    {
        std::cout << "User is exist!" << std::endl;
        return false;
    }
    else
    {
        std::string sqlQueryCommand;
        std::string values;
        values = "\"" + clientUsername + "\", " + "\"" + clientPassword + "\", " + "\"" + emailClient + "\", " + "\"" + fullnameClient + "\"" + ", 0";
        sqlQueryCommand = "INSERT INTO USER VALUES (" + values + ")";
        this->queryMySQLHandler(sqlQueryCommand);
    }
    return true;
}

// Query password, check if it user is existed or not
// if user exist return password, else return empty string
std::string Database::queryUserHandler(std::string clientUsername)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT password FROM USER WHERE username=";
    sqlQueryCommand.append("\"" + clientUsername + "\"");
    // row contains user information
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    if (row == NULL)
    {
        mysql_free_result(selectSet);
        return "";
    }
    std::string password = std::string(row[0]);
    mysql_free_result(selectSet);
    return password;
}

// function to send conversation details to server
std::string Database::sendConversationDetails(struct Conversation conversation[], int arrLength)
{
    std::string message = "";
    for (int k = 0; k < arrLength; k++)
    {
        message += std::to_string(conversation[k].ID) + "$";
        message += conversation[k].name + "$";
        message += std::to_string(conversation[k].numberOfPartipants) + "$";
        message += "|";
    }
    return message;
}

// function to query conversation and send conversation to server
std::string Database::queryConversation(std::string username)
{
    std::string sqlQueryCommand;
    std::string curConversationID = "";
    Conversation conversation[10];
    sqlQueryCommand = "select conversationID from PARTICIPATE\
                                                where username = \"" +
                      username + "\"";
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row;
    std::string conversationIDs[10];
    int totalConversation = 0;

    while ((row = mysql_fetch_row(selectSet)) != NULL)
    {
        conversationIDs[totalConversation] = std::string(row[0]);
        totalConversation += 1;
    }
    mysql_free_result(selectSet);
    int curPosition = 0;
    while (curPosition < totalConversation)
    {
        if (conversationIDs[curPosition] != curConversationID)
        {
            curConversationID = conversationIDs[curPosition];
            conversation[curPosition].ID = stoi(conversationIDs[curPosition]);
            this->queryConversationDetails(curConversationID, conversation, curPosition);
        }
        curPosition++;
    }

    return this->sendConversationDetails(conversation, totalConversation);
}

// function to query message history
std::string Database::queryMessageHandler(std::string conversationID)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT S.username, M.text, C.conversation_name\
                    FROM (SEND as S JOIN MESSAGE as M on S.messageID = M.messageID) JOIN CONVERSATION as C on S.conversationID = C.conversationID\
                    WHERE C.conversationID = " +
                      conversationID;
    this->queryMySQLHandler(sqlQueryCommand);
    std::string message = "";
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row;
    // format string return
    // conversation_name$username$text\n$sername$text\n$
    std::string conversation_name = "";
    int flagConversationName = 0;
    while ((row = mysql_fetch_row(selectSet)))
    {
        if (flagConversationName == 0)
        {
            message += std::string(row[2]) + "$";
            flagConversationName++;
        }
        message += std::string(row[0]) + "$";
        message += std::string(row[1]) + '\n';
    }
    // mysql_free_result(selectSet);
    if (message == "")
    {
        message += "New conversation$";
        std::string conversation_name = this->queryConversationName(conversationID);
        message += conversation_name;
    }
    return message;
}

// function to authenticate user when user login
bool Database::authenticateLogin(std::string clientUsername, std::string clientPassword)
{
    std::string passwordFromDB = this->queryUserHandler(clientUsername);
    if (passwordFromDB == "")
    {
        std::cout << "Username is not exist!" << std::endl;
        return false;
    }
    else if (passwordFromDB != clientPassword)
    {
        std::cout << "Wrong password!" << std::endl;
        return false;
    }
    std::cout << "Login success!" << std::endl;
    return true;
}

// function to insert new conversation, return conversationID of new conversation insert
std::string Database::insertConversation(std::string conversationName)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "INSERT INTO CONVERSATION (no_participants, no_active_p, conversation_name) VALUES (";
    sqlQueryCommand.append("0, 0,\"" + conversationName + "\");");
    this->queryMySQLHandler(sqlQueryCommand);
    sqlQueryCommand = "SELECT MAX(conversationID) from CONVERSATION WHERE conversation_name = \"";
    sqlQueryCommand.append(conversationName + "\";");
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    std::string result = std::string(row[0]);
    mysql_free_result(selectSet);
    return result;
}

// function to get number of participants
int Database::queryNumberOfParticipants(std::string conversationID)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT no_participants FROM CONVERSATION WHERE conversationID=";
    sqlQueryCommand.append("\"" + conversationID + "\";");
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    int numberOfParicipants = convertToInteger(row[0]);
    mysql_free_result(selectSet);
    return numberOfParicipants;
}

// function to get conversation name by conversation
std::string Database::queryConversationName(std::string conversationID)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT conversation_name FROM CONVERSATION WHERE conversationID=";
    sqlQueryCommand.append("\"" + conversationID + "\";");
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    std::string conversatioName = "";
    if (row != NULL)
    {
        conversatioName = std::string(row[0]);
    }
    mysql_free_result(selectSet);
    return conversatioName;
}

// function to get member list in conversation
void Database::queryConversationParticipants(std::string conversationID, std::vector<std::string> &participants)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT username FROM PARTICIPATE WHERE conversationID=";
    sqlQueryCommand.append("\"" + conversationID + "\";");
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(selectSet)))
    {
        participants.push_back(std::string(row[0]));
    }
    mysql_free_result(selectSet);
}

// function to get conversation detail
int Database::queryConversationDetails(std::string conversationID, Conversation conversation[], int curPosition)
{
    // get number of participate
    conversation[curPosition].numberOfPartipants = this->queryNumberOfParticipants(conversationID);
    // get conversation Name
    conversation[curPosition].name = this->queryConversationName(conversationID);
    // get conversation participants;
    return 1;
}

// function to insert user in participate table
bool Database::insertParticipate(std::string conversationID, std::string username)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "INSERT INTO PARTICIPATE VALUES (";
    sqlQueryCommand.append("\"" + conversationID + "\", \"" + username + "\")");
    this->queryMySQLHandler(sqlQueryCommand);

    // update number of participate in conversation
    std::string updateNumberofP;
    int numberOfParticipate = this->queryNumberOfParticipants(conversationID);
    updateNumberofP = "UPDATE CONVERSATION SET no_participants=";
    updateNumberofP.append("\"" + std::to_string(numberOfParticipate + 1) + "\" WHERE CONVERSATIONID=\"" + conversationID + "\";");
    this->queryMySQLHandler(updateNumberofP);

    return true;
}

// function to insert admin in group chat
bool Database::insertAdministrate(std::string conversationID, std::string username)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "INSERT INTO ADMINISTRATE VALUES (";
    sqlQueryCommand.append("\"" + conversationID + "\", \"" + username + "\")");
    this->queryMySQLHandler(sqlQueryCommand);
    return true;
}

// function to check username and password is exist in PARTICIPATE
// if exist return true
// else return false
bool Database::checkUsernameIsExistInParticipate(std::string conversationID, std::string username)
{
    std::string statement;
    statement = "SELECT * FROM PARTICIPATE WHERE conversationID=";
    statement.append("\"" + conversationID + "\" && username=\"" + username + "\"");
    this->queryMySQLHandler(statement);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);

    if (row != NULL)
    {
        std::cout << "Username and ConversationID is exist" << std::endl;
        mysql_free_result(selectSet);
        return true;
    }
    else
    {
        mysql_free_result(selectSet);
        return false;
    }
    return false;
}

// function insert new conversation
std::string Database::insertConversationHandler(std::string conversationName, std::string userCreator)
{
    std::string conversationID = this->insertConversation(conversationName);
    this->insertParticipate(conversationID, userCreator);
    return conversationID;
}

// function to insert new message and messageID auto increment
bool Database::insertMessageHandler(std::string username, std::string conversationID, std::string message)
{
    // insert message in MESSAGE TABLE
    std::string insertMessageStatement;
    insertMessageStatement = "INSERT INTO MESSAGE (text) VALUE (";
    char *messageEscape = new char[strlen(message.c_str()) * 3 + 1];
    mysql_escape_string(messageEscape, message.c_str(), message.length());
    std::string messageEscapeString(messageEscape);
    insertMessageStatement.append("\"" + messageEscapeString + "\")");
    this->queryMySQLHandler(insertMessageStatement);

    // select max messageID
    std::string selectMessageIDStatement;
    selectMessageIDStatement = "SELECT max(messageID) FROM MESSAGE";
    this->queryMySQLHandler(selectMessageIDStatement);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    std::string newMessageID = row[0];
    mysql_free_result(selectSet);

    // insert information in SEND TABLE after insert MESSAGE TABLE
    std::string insertSendStatemnet;
    insertSendStatemnet = "INSERT INTO SEND VALUES (";
    insertSendStatemnet.append("\"" + newMessageID + "\"," + "\"" + conversationID + "\", " + "\"" + username + "\")");
    this->queryMySQLHandler(insertSendStatemnet);
    return true;
}

// function to check private chat exist or not
bool Database::checkPrivateChatIsExist(std::string username1, std::string username2)
{
    std::string statementCheckPrivateChatIsExist = "SELECT conversation_name FROM CONVERSATION AS c INNER JOIN PARTICIPATE AS p ON c.conversationID = p.conversationID WHERE username = \"" + username1 + "\" and no_participants = \"2\" INTERSECT SELECT conversation_name FROM CONVERSATION AS c INNER JOIN PARTICIPATE AS p ON c.conversationID = p.conversationID WHERE username = \"" +
                                                   username2 + "\" and no_participants = \"2\"";

    this->queryMySQLHandler(statementCheckPrivateChatIsExist);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    std::string conversationName = "";
    if (row != NULL)
    {
        mysql_free_result(selectSet);
        return true;
    }
    mysql_free_result(selectSet);
    return false;
}

// function to check list user have a common group or not
bool Database::checkPublicChatIsExist(std::string numberOfUser, std::string usernameList, std::string conversationName)
{
    std::string statementCheckPublicChatIsExist = "SELECT p.conversationID FROM CONVERSATION AS c INNER JOIN PARTICIPATE AS p ON c.conversationID = p.conversationID WHERE c.conversation_name = \"" + conversationName + "\" AND " + "no_participants = \"" + numberOfUser + "\"" + " AND " + "username IN (" + usernameList + ");";
    this->queryMySQLHandler(statementCheckPublicChatIsExist);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    int i = 0;
    MYSQL_ROW row;
    std::vector<int> vectorConversationID;
    while (row = mysql_fetch_row(selectSet))
    {
        vectorConversationID.push_back(atoi(row[0]));
    }
    std::sort(vectorConversationID.begin(), vectorConversationID.end());
    for (int i = 0; i < vectorConversationID.size(); i++)
    {
        if (std::count(vectorConversationID.begin(), vectorConversationID.end(), vectorConversationID.at(i)) == stoi(numberOfUser))
        {
            return true;
        }
    }
    mysql_free_result(selectSet);
    return false;
}

// function to delete user from participate
bool Database::deleteUserInParticipate(std::string conversationID, std::string username)
{
    std::string sqlDeleteUserInConversation;
    sqlDeleteUserInConversation = "DELETE FROM PARTICIPATE WHERE conversationID = ";
    sqlDeleteUserInConversation.append("\"" + conversationID + "\" AND username = \"" + username + "\"");
    this->queryMySQLHandler(sqlDeleteUserInConversation);

    // update number of participate in conversation
    std::string updateNumberofP;
    int numberOfParticipate = this->queryNumberOfParticipants(conversationID);
    updateNumberofP = "UPDATE CONVERSATION SET no_participants=";
    updateNumberofP.append("\"" + std::to_string(numberOfParticipate - 1) + "\" WHERE CONVERSATIONID=\"" + conversationID + "\";");
    this->queryMySQLHandler(updateNumberofP);

    return true;
}

// function to check user have admin rights or not
bool Database::queryAdministrateHandler(std::string conversationID, std::string username)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT * FROM ADMINISTRATE WHERE username=";
    sqlQueryCommand.append("\"" + username + "\" AND conversationID = " + conversationID);
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    if (row == NULL)
    {
        mysql_free_result(selectSet);
        return false;
    }
    mysql_free_result(selectSet);
    return true;
}

std::string Database::queryListUserHandler()
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT username FROM USER";
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    std::string listUsername = "";
    int countNumberUsername = 0;
    while ((row = mysql_fetch_row(selectSet)))
    {
        listUsername += std::string(row[0]) + "$";
        countNumberUsername++;
    }
    std::string listUsernameSendToServer = "LU$";
    listUsernameSendToServer += std::to_string(countNumberUsername);
    listUsernameSendToServer += "$" + listUsername;
    mysql_free_result(selectSet);
    return listUsernameSendToServer;
}
