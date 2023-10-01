#include "db.h"

bool Database::connectMySQLHandler()
{
    this->connection = mysql_init(NULL);
    if (!mysql_real_connect(this->connection, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->schema.c_str(), 0, NULL, 0))
    {
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    return true;
}

std::string Database::queryMySQLHandler(std::string sqlQueryCommand)
{
    // point to query command string, create a result set
    int queryState = mysql_query(this->connection, sqlQueryCommand.c_str());
    if (queryState)
    {
        std::cout << "MySQL Query Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    int firstSpacePos = sqlQueryCommand.find(' ');
    std::string commandType = sqlQueryCommand.substr(0, firstSpacePos);
    return commandType;
}

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
std::string Database::queryUserHandler(std::string clientUsername)
{
    std::cout << clientUsername << "\n";
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

std::string Database::sendConversationDetails(struct Conversation conversation[], int arrLength)
{
    std::string message = "";
    for (int k = 0; k < arrLength; k++)
    {
        message += std::to_string(conversation[k].ID) + "$";
        message += conversation[k].name + "$";
        // std::cout << "Conversation participants: ";
        for (auto i = conversation[k].participants.begin(); i != conversation[k].participants.end(); i++)
        {
            message += *i + "$";
        }
        message += "|";
    }
    return message;
}

std::string Database::queryConversation(std::string username)
{
    std::string sqlQueryCommand;
    std::string curConversationID = "";
    Conversation conversation[10];
    sqlQueryCommand = "select conversationID from PARTICIPATE\
                                                where username = \"" +
                      username + "\"";
    // std::cout << username << " Hello\n";

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
            curConversationID = conversationIDs[curPosition]; // ID at 0th
            conversation[curPosition].ID = stoi(conversationIDs[curPosition]);
            this->queryConversationDetails(curConversationID, conversation, curPosition);
        }
        curPosition++;
    }

    return this->sendConversationDetails(conversation, totalConversation);
}

std::string Database::queryMessageHandler(std::string conversationID)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "SELECT S.username, M.text\
                    FROM (SEND as S JOIN MESSAGE as M on S.messageID = M.messageID) JOIN CONVERSATION as C on S.conversationID = C.conversationID\
                    WHERE C.conversationID = " +
                      conversationID;
    this->queryMySQLHandler(sqlQueryCommand);
    std::string message = "";
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(selectSet)))
    {
        message += std::string(row[0]) + "$";
        message += std::string(row[1]) + '\n';
    }
    if (message == "")
    {
        message = "New conversation";
    }
    return message;
}

// check login
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

// Sửa lại tham số truyền vào, cũng như câu lệnh khởi tạo conversation (Hỏi những người xử lý client)
std::string Database::insertConversation(std::string conversationName)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "INSERT INTO CONVERSATION (no_participants, no_active_p, conversation_name) VALUES (";
    sqlQueryCommand.append("2, 2,\"" + conversationName + "\");");
    this->queryMySQLHandler(sqlQueryCommand);
    sqlQueryCommand = "SELECT conversationID from CONVERSATION WHERE conversation_name = \"";
    sqlQueryCommand.append(conversationName + "\";");
    this->queryMySQLHandler(sqlQueryCommand);
    MYSQL_RES *selectSet = mysql_use_result(this->connection);
    MYSQL_ROW row = mysql_fetch_row(selectSet);
    std::string result = std::string(row[0]);
    mysql_free_result(selectSet);
    return result;
}

// getNumberOfParticipate
int Database::queryNumberOfPartipants(std::string conversationID)
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

// get conversation Name
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

int Database::queryConversationDetails(std::string conversationID, Conversation conversation[], int curPosition)
{
    // get number of participate
    conversation[curPosition].numberOfPartipants = this->queryNumberOfPartipants(conversationID);
    std::cout << conversation[curPosition].numberOfPartipants << std::endl;
    // get conversation Name
    conversation[curPosition].name = this->queryConversationName(conversationID);
    // get conversation participants;
    this->queryConversationParticipants(conversationID, conversation[curPosition].participants);
    return 1;
}

bool Database::insertParticipate(std::string conversationID, std::string username)
{
    std::string sqlQueryCommand;
    sqlQueryCommand = "INSERT INTO PARTICIPATE VALUES (";
    sqlQueryCommand.append("\"" + conversationID + "\", \"" + username + "\")");
    this->queryMySQLHandler(sqlQueryCommand);

    // update number of participate in conversation
    std::string updateNumberofP;
    int numberOfParticipate = this->queryNumberOfPartipants(conversationID);
    updateNumberofP = "UPDATE CONVERSATION SET no_participants=";
    updateNumberofP.append("\"" + std::to_string(numberOfParticipate + 1) + "\" WHERE CONVERSATIONID=\"" + conversationID + "\";");
    this->queryMySQLHandler(updateNumberofP);

    return true;
}

// Check username and password is exist in PARTICIPATE
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

// insert new conversation
bool Database::insertConversationHandler(std::string conversationName, std::string userCreator)
{
    // conversation is not exist
    if (this->queryConversationName(conversationName) == "")
    {
        std::string conversationID = this->insertConversation(conversationName);
        this->insertParticipate(conversationID, userCreator);
        // this->insertParticipate(conversationID, userReceiver);
        return true;
    }
    /*
    else if (this->checkUsernameIsExistInParticipate(conversationID, username))
    {
        return true;
    }
    else
    {
        this->insertParticipate(conversationID, username);
    }
    */
    return true;
}

// insert new message
// messageID auto increment
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
