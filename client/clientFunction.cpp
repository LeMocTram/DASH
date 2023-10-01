#include "client.h"

struct clientStruct staticClient;

bool Client::isValidEmail(const std::string &email)
{
    // regex expression to check email validity
    // example email : example@gmail.com
    std::regex emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return std::regex_match(email, emailRegex); // to compare string input with regex, return true or false
}

bool Client::isValidUsername(const std::string &username)
{
    // regex expression to check email validity
    // contains only alphanumeric form 5 to 20 characters
    std::regex usernameRegex("^[a-zA-Z0-9]{5,20}$");
    return std::regex_match(username, usernameRegex); // to compare string input with regex, return true or false
}
bool Client::isValidFullname(const std::string &fullname)
{
    // regex expression to check email validity
    // contains only alphanumeric form 5 to 20 characters
    std::regex fullnameRegex("^[a-zA-Z]{0,30}$");
    return std::regex_match(fullname, fullnameRegex); // to compare string input with regex, return true or false
}

bool Client::isValidPassword(const std::string &password)
{
    // regex expression to check password validity
    // At least 6 characters, must have at least one uppercase letter, one lowercase letter and a number
    std::regex passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9]).{6,}$");
    return std::regex_match(password, passwordRegex); // to compare string input with regex, return true or false
}

bool Client::isValidNumber(const std::string &number)
{
    std::regex numberRegex("^[1-9][0-9]*"); // contains only number
    return std::regex_match(number, numberRegex);
}

std::string Client::enterUsername()
{
    std::getline(std::cin, clientInformation.username);
    if (clientInformation.username == ":return")
    {
        return ":return";
    }
    while (!isValidUsername(clientInformation.username))
    {
        clientInformation.username.clear();
        std::cerr << "\033[1m\033[33mUsername contains only alphanumeric form 5 to 20 characters.\nPlease re-enter\033[0m \n\e[1mUsername: \033[0m";
        std::getline(std::cin, clientInformation.username);
        if (clientInformation.username == ":return")
        {
            return ":return";
        }
    }
    return clientInformation.username;
}
std::string Client::enterPassword()
{
    char password[100];
    std::snprintf(password, sizeof(password), "%s", getpass("\e[1mPassword: \e[0m"));
    clientInformation.password = std::string(password);
    if (clientInformation.password == ":return")
    {
        return ":return";
    }
    while (!isValidPassword(clientInformation.password))
    {
        clientInformation.password.clear();
        std::cerr << "\033[1m\033[33mAt least 6 characters, must have at least one uppercase letter, one lowercase letter and a number\nPlease re-enter\033[0m" << std::endl;
        char password[100];
        std::snprintf(password, sizeof(password), "%s", getpass("\e[1mPassword: \e[0m"));
        clientInformation.password = std::string(password);
        if (clientInformation.password == ":return")
        {
            return ":return";
        }
    }
    return clientInformation.password;
}
std::string Client::enterEmail()
{
    std::cout << "\e[1mEmail   : \e[0m";
    std::getline(std::cin, clientInformation.email);
    if (clientInformation.email == ":return")
    {
        return ":return";
    }
    while (!isValidEmail(clientInformation.email))
    {
        clientInformation.email.clear();
        std::cerr << "\033[1m\033[33mInvalid email address. Example: example@gmail.com.\nPlease re-enter.\033[0m \n\e[1mEmail   : \e[0m";
        std::getline(std::cin, clientInformation.email);
        if (clientInformation.email == ":return")
        {
            return ":return";
        }
    }
    return clientInformation.email;
}
std::string Client::enterFullname()
{
    clientInformation.fullname.clear();
    std::cout << "\e[1mFullname: \e[0m";
    std::getline(std::cin, clientInformation.fullname);
    if (clientInformation.fullname == ":return")
    {
        return ":return";
    }
    while (!isValidFullname(clientInformation.fullname))
    {
        clientInformation.email.clear();
        std::cerr << "\033[1m\033[33mFullname contains only alpha.\nPlease re-enter.\033[0m \n\e[1mFullname: \e[0m";
        std::getline(std::cin, clientInformation.fullname);
        if (clientInformation.fullname == ":return")
        {
            return ":return";
        }
    }
    return clientInformation.email;
}

// Erase line
void Client::eraseLines(int count)
{
    if (count > 0)
    {
        std::cout << "\x1b[2K"; // Delete current line
        // i=1 because we included the first line
        // Ham chua hop ly lam :))sd
        for (int i = 1; i < count; i++)
        {
            std::cout
                << "\x1b[1A"  // Move cursor up one
                << "\x1b[2K"; // Delete the entire line
        }
        std::cout << "\r"; // Resume the cursor at beginning of line
    }
}

// Function to create a client socket
int Client::createClientSocket()
{
    // Create client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        return clientSocket;
    }
    return clientSocket;
}

// Function to connect to the server
bool Client::connectToServer(int clientSocket, const std::string &ipAddress, int port)
{
    // Set server IP, PORT
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);

    // Connect client to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        return false;
    }
    return true;
}

// Function to hash the password, use SHA 512, return hash string.
std::string Client::hashPassword(std::string username, std::string password)
{
    // append username and password
    std::string hashString = username + password;

    // convert string to unsigned char
    unsigned char *data = (unsigned char *)hashString.c_str();
    unsigned char hash[SHA512_DIGEST_LENGTH];

    // hash string
    SHA512(data, strlen((char *)data), hash);
    std::string hashResultString = "";

    // process result to string
    for (int i = 0; i < 64; i++)
    {
        char buffer[4];
        snprintf(buffer, 4, "%02x", hash[i]);
        std::string test(buffer);
        hashResultString += test;
    }

    return hashResultString;
}

// Register account
bool Client::registerAccount(int clientSocket, std::string registerMessage)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    send(clientSocket, registerMessage.c_str(), registerMessage.length(), 0);
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0)
    {
        this->reconnectToServer(clientSocket);
        std::cout << "\033[1m\033[33mPlease register again!!\033[0m\n";
    }
    if (strcmp(buffer, "no") == 0)
    {
        return false;
    }
    return true;
}

// Function to get user input and log in to the server.
int Client::login(int clientSocket, std::string loginMessage)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    send(clientSocket, loginMessage.c_str(), loginMessage.length(), 0);
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0)
    {
        this->reconnectToServer(clientSocket);
        std::cout << "\033[1m\033[33mPlease login again!!\033[0m\n";
    }
    if (strcmp(buffer, "no") == 0)
    {
        return false;
    }
    return true;
}

// List user from the chat application
// This is for create conversation
bool Client::listUser(int clientSocket)
{
    system("clear");
    std::string listUser = "LU";
    std::string listUserMessage;
    char buffer[10000];

checkBackListUser:
    memset(buffer, 0, sizeof(buffer));
    // format listUser = "LU"
    send(clientSocket, listUser.c_str(), listUser.length(), 0);
    sleep(1);
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    // format listUserMessage = "LU$numberOfUser$user1$user2$user3$....."
    listUserMessage = std::string(buffer);
    memset(buffer, 0, sizeof(buffer));
    // format numberOfUser = "numberOfUser"
    int first, second;
    first = listUserMessage.find("$");
    second = listUserMessage.find("$", first + 1);
    std::string checkIfList = listUserMessage.substr(0, first);
    std::string numberOfUser = listUserMessage.substr(first + 1, second - (first + 1));

    if (bytesRead <= 0)
    {
        this->reconnectToServer(clientSocket);
    }
    if (checkIfList != "LU")
    {
        goto checkBackListUser;
    }

    else
    {
        // format numberUser = (int) numberOfUser
        int numberUser = atoi(numberOfUser.c_str());
        int fpartPos, spartPos;
        // format user = "$user1$user2$user3$....."
        std::string user = listUserMessage.substr(second);
        srand(time(0));
        int ran, count = 0;
        fpartPos = 0;
        std::cout << "\033[1m\033[32mList of suggested users:\n";
        std::cout << "---------------------------\033[0m\n";
        do
        {
            // Random user to list if numberOfUser > 5
            spartPos = user.find('$', fpartPos + 1);
            if (numberUser >= 5)
            {
                // ran = rand_r(*seed) % (spartPos - fpartPos) + fpartPos;
                ran = 111;
            }
            else
            {
                ran = 10000;
            }
            if (ran > fpartPos)
            {
                std::string displayUser = user.substr(fpartPos + 1, spartPos - (fpartPos + 1));
                std::cout << displayUser;
                if (displayUser == USERNAME)
                {
                    std::cout << " (me)";
                }
                std::cout << "\n";
                count++;
            }
            else
            {
                spartPos = user.find('$', spartPos + 1);
                if (spartPos <= -1)
                {
                    break;
                }
            }
            fpartPos = spartPos;
        } while (user.find('$', fpartPos + 1) != -1 && count < 5);
    }
    std::cout << "\033[1m\033[32m---------------------------\033[0m\n\n";
    return true;
}

std::string Client::chooseTypeOfConversation()
{
    std::cout << "\033[1m\033[32mPress 1 to create private chat" << std::endl;
    std::cout << "Press 2 to create public chat" << std::endl;
    std::cout << "\033[1m\033[31mPress 3 to return\n\033[39m"
              << std::endl;

    int value = getch();
    while (value != 49 && value != 50 && value != 51)
    {
        std::cerr << "\033[1m\033[31mIncorrect, re-enter:\n\033[39m";
        value = getch();
    }
    if (value == 49)
    {
        return "private";
    }
    if (value == 51)
    {
        return "exit";
    }
    return "group";
}

// Function to create conversation
void Client::createConversation(int clientSocket)
{
    // system("clear");
    char buffer[1024];
    // Choose type of conversation
    std::string typeOfConversation;
    typeOfConversation = this->chooseTypeOfConversation();

    do
    {
        if (typeOfConversation == "exit")
        {
            break;
        }

        std::string createConversationMessage = "CC", username;
        std::string usernameListString = "";
        memset(buffer, 0, sizeof(buffer));

        // Create conversation
        if (typeOfConversation == "group")
        {
            std::cout << "\033[1m\033[32m________Public Chat________\033[0m\n";
            std::cout << "\033[1m\033[37mEnter conversation name: \033[0m";
            std::getline(std::cin, conv.name);
            while (!isValidUsername(conv.name))
            {
                conv.name.clear();
                std::cerr << "\033[1m\033[33mThe name of conversation contains only alphanumeric form 5 to 20 characters." << std::endl;
                std::cerr << "Please re-enter name: \033[0m" << std::endl;
                std::getline(std::cin, conv.name);
            }
        }
        if (typeOfConversation == "private")
        {
            std::cout << "\033[1m\033[32m________Private Chat________\033[0m\n";
        }

        // Add user in private chat or group chat
        int numberOfUser = 0;
        bool checkUserIsDuplicate = true;
        std::vector<std::string> usernameList;
        usernameList.push_back(USERNAME);
        while (true)
        {
            std::cout << "\033[1m\033[37mUsername participant:\033[0m";
            std::getline(std::cin, username);
            while (!isValidUsername(username))
            {
                username.clear();
                std::cerr << "\033[1m\033[33mUsername contains only alphanumeric form 5 to 20 characters";
                std::cout << "Re-enter username: \033[0m" << std::endl;
                std::getline(std::cin, username);
            }
            if (std::find(usernameList.begin(), usernameList.end(), username) == usernameList.end())
            {
                usernameListString += username + "$";
                usernameList.push_back(username);
                checkUserIsDuplicate = false;
            }
            else
            {
                checkUserIsDuplicate = true;
                std::cerr << username << "\033[1m\033[31m The user already exists in the group.\033[0m" << std::endl;
            }

            if ((typeOfConversation == "private") && (checkUserIsDuplicate == false))
            {
                numberOfUser += 1;
                break;
            }
            if ((typeOfConversation == "group") && (checkUserIsDuplicate == false))
            {
                numberOfUser += 1;
                if (numberOfUser >= 2)
                {
                    std::cout << "\033[1m\033[32mDo you want to add more member?" << std::endl;
                    std::cout << "Press 1 to create." << std::endl;
                    std::cout << "Press 2 to continously add user.\033[0m" << std::endl;

                    int value = getch();

                    while (value != 49 && value != 50)
                    {
                        std::cerr << "\033[1m\033[31mIncorrect, re-enter your option.\033[0m";
                        value = getch();
                    }
                    if (value == 49)
                    {
                        break;
                    }
                }
            }
        }

        // Send insert message to server
        if (typeOfConversation == "group")
        {
            createConversationMessage = createConversationMessage + "$" + conv.name + "$" + std::to_string(numberOfUser) + "$" + usernameListString;
        }
        else
        {
            createConversationMessage = createConversationMessage + "$" + USERNAME + "-" + username + "$" + std::to_string(numberOfUser) + "$" + usernameListString;
        }
        send(clientSocket, createConversationMessage.c_str(), createConversationMessage.length(), 0);
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0)
        {
            this->reconnectToServer(clientSocket);
            std::cout << "\033[1m\033[33mPlease create conversation again.\033[0m\n";
        }

        if (strcmp(buffer, "no") == 0)
        {
            std::cerr << "\033[1m\033[31mCreate failed! Username not exist!\033[0m\n";
        }
        if (strcmp(buffer, "groupIsExist") == 0)
        {
            std::cerr << "\033[1m\033[31mThe group name must be different!\033[0m\n";
        }
        if (strcmp(buffer, "privateChatIsExist") == 0)
        {
            std::cerr << "\033[1m\033[31mThe private chat is exist!\033[0m\n";
        }
    } while (strcmp(buffer, "yes"));
    std::cout << "Create success!\n";
}

// Function to display conversation
void Client::getConversation(int clientSocket)
{
    char receiveConv[1024];
    std::string sendQueryConv = "QC";

    this->conv.name.clear();
    this->conv.participants.clear();
    this->conv.ID = 0;

    // std::string queryConversation = "10$ChatName$Khanh$Hung$Duy$Tram$Trung$";
    send(clientSocket, sendQueryConv.c_str(), sendQueryConv.length(), 0);
    sleep(1);
    std::cout << "\033]0;"
              << "LIST CONVERSATION"
              << "\007";
    std::cout << "\033[1m\033[37m+----------------------------------------------------------------+" << std::endl;
    std::cout << '|' << std::setw(17) << std::left << " Conversation ID "
              << '|' << std::setw(25) << std::left << "        CHAT" << '|' << std::setw(20) << std::left << "        TYPE" << '|' << std::endl;
    std::cout << "+----------------------------------------------------------------+\033[0m" << std::endl;
    do
    {
        memset(receiveConv, 0, sizeof(receiveConv));
        int bytesRead = recv(clientSocket, receiveConv, sizeof(receiveConv), 0);
        // sleep(1);
        if (bytesRead <= 0)
        {
            this->reconnectToServer(clientSocket);
            break;
        }
        // std::cout << std::string(receiveConv);
        std::string queryConversation = std::string(receiveConv);
        if (queryConversation != "End conversation")
        {
            size_t first, second, fpartPos, spartPos;
            first = queryConversation.find_first_of('$');

            // query conversation format = convID$convName$Participant1$participant2$participant3$.........
            // Number of participant is not known in advanced
            this->conv.name.clear();
            this->conv.participants.clear();
            this->conv.ID = atoi(queryConversation.substr(0, first).c_str());
            this->conv.numberOfPartipants = 0;
            second = queryConversation.find('$', first + 1);
            this->conv.name = queryConversation.substr(first + 1, second - (first + 1));

            fpartPos = second;
            if (this->conv.name != "yes" && this->conv.name != "no")
            {
                if (first != -1 && second != -1)
                {
                    std::cout << '|' << std::setw(17) << "        " + std::to_string(this->conv.ID) << "|" << std::setw(25) << "  " + this->conv.name << '|' << std::setw(20);
                    this->conv.numberOfPartipants = stoi(queryConversation.substr(second + 1, queryConversation.find('$', second + 1) - (second + 1)));
                }
                else
                {
                    break;
                }
                /*
                do
                {
                    spartPos = queryConversation.find('$', fpartPos + 1);
                    this->conv.participants.push_back(queryConversation.substr(fpartPos + 1, spartPos - (fpartPos + 1)));
                    fpartPos = spartPos;
                    this->conv.numberOfPartipants++;
                } while (queryConversation.find('$', fpartPos + 1) != -1);
                */
                if (this->conv.numberOfPartipants == 2)
                {
                    std::cout << "      private";
                }
                else
                {
                    std::cout << "      public";
                }
            }
            std::cout << "|\n";
        }
    } while (std::string(receiveConv) != "End conversation");
    std::cout << "\033[1m\033[37m+----------------------------------------------------------------+\033[0m" << std::endl;
}
// Function choose conversation
std::string Client::chooseConversation(int clientSocket)
{
    char buffer[10000];
    std::string idInput, idSend;
    int bytesRead;
    do
    {
        std::cout << "\033[1m\033[34mEnter ID of the Conversation: \033[0m";
        std::cin >> idInput;
        std::cin.ignore();
        idSend = "ID" + idInput;
        if (!isValidNumber(idInput))
        {
            std::cerr << "\033[1m\033[31mID is wrong. Please re-enter. \033[0m";
        }
    } while (!isValidNumber(idInput));
    send(clientSocket, idSend.c_str(), idSend.length(), 0);
    memset(&buffer, 0, sizeof(buffer));
    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    // Check if recv a conversation in format sender$conversatioNID$message
    int findConvID = std::string(buffer).find('$');
    std::string checkIfConvID = std::string(buffer).substr(findConvID + 1, 1);
    if (isdigit(checkIfConvID.c_str()[0]))
    {
        send(clientSocket, idSend.c_str(), idSend.length(), 0);
        memset(&buffer, 0, sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    }

    if (bytesRead <= 0)
    {
        this->reconnectToServer(clientSocket);
        send(clientSocket, idSend.c_str(), idSend.length(), 0);
        memset(&buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);
    }

    int firstPointer = 0, secondPointer = 0;
    system("clear");

    if (strcmp(buffer, "New conversation") == 0)
    {
        std::cout << "New conversation\n";
    }

    else
    {
        while (strcmp(buffer, "no") == 0)
        {
            memset(buffer, 0, sizeof(buffer));
            do
            {
                std::cerr << "Invalid ID. Pls re-enter: ";
                std::cin >> idInput;
                std::cin.ignore();
                idSend = "ID" + idInput;
            } while (!isValidNumber(idInput));
            send(clientSocket, idSend.c_str(), idSend.length(), 0);
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0)
            {
                this->reconnectToServer(clientSocket);
                send(clientSocket, idSend.c_str(), idSend.length(), 0);
                memset(&buffer, 0, sizeof(buffer));
                recv(clientSocket, buffer, sizeof(buffer), 0);
            }
        }
        this->getMessageHistory(std::string(buffer));
    }
    memset(&buffer, 0, sizeof(buffer));
    // memset(&conversationNameBuffer, 0, sizeof(conversationNameBuffer));
    return idInput;
}

bool Client::getMessageHistory(std::string message)
{
    if (message.find("New conversation") == 0)
    {
        std::string conversation_name = message.substr(17);
        std::cout << "\033]0;"
                  << conversation_name
                  << "\007";
        std::cout << "New conversation\n";
    }

    else
    {
        // Get conversation_name
        // format of message string
        // conversation_name$username$text\n$username$text\n$username$text\n
        std::string conversation_name = "";
        conversation_name = message.substr(0, message.find("$"));
        message = message.substr(message.find("$") + 1);
        std::cout << "\033]0;"
                  << conversation_name
                  << "\007";
        int secondPointer, firstPointer;
        for (int i = 0; i < message.length(); i = ++firstPointer)
        {
            secondPointer = i;
            firstPointer = i;
            while (message[secondPointer] != '$')
            {
                secondPointer++;
            }
            message.replace(secondPointer, 1, ": ");
            firstPointer = secondPointer;
            while (message[firstPointer] != '\n')
            {
                firstPointer++;
            }
            if (message.substr(i, secondPointer - i) == USERNAME)
            {
                message.replace(i, secondPointer - i + 1, "Me:");
                firstPointer = firstPointer - USERNAME.length() + 2;
            }
        }
        std::cout << message << std::endl;
        message.clear();
    }
    return true;
}

// Function to close the client socket
void Client::closeClientSocket(int clientSocket)
{
    close(clientSocket);
}

// Function to receive and display messages from the server
void receiveMessage(int clientSocket, int conversationID)
{
    // Receive message flow
    Client client;

    while (flag_atomic)
    {
        if (!flag_atomic)
        {
            break;
        }
        char buffer[1024];

        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0)
        {
            client.reconnectToServer(clientSocket);
        }
        std::string message = std::string(buffer);
        if (buffer[0] == 'Q')
        {
            int startPos = 0, endPos = 0;
            std::string message = std::string(buffer).substr(1);
            std::cout << "\033[1m\033[34m___________Participant List___________\033[0m\n";
            while ((endPos = message.find('$', startPos)) != -1)
            {
                std::cout << "\t\t" << message.substr(startPos, endPos - startPos) << std::endl;
                startPos = endPos + 1;
            }
            std::cout << "\033[1m\033[34m________________________________________\033[0m\n";
            memset(buffer, 0, sizeof(buffer));
            std::cout << std::flush;
        }
        else if (buffer[0] == 'A')
        {
            std::string message = std::string(buffer).substr(1);
            std::cout << message << std::endl;
            memset(buffer, 0, sizeof(buffer));
            std::cout << std::flush;
        }
        else if (buffer[0] == 'D')
        {
            std::string message = std::string(buffer).substr(1);
            std::cout << message << std::endl;
            memset(buffer, 0, sizeof(buffer));
            std::cout << std::flush;
        }
        else
        {
            // Message format receiveMessage = sender$conversationID$message
            size_t first, second;
            // find first $
            first = message.find('$');
            // find seconnd $
            second = message.find('$', first + 1);
            // get sender = sender
            std::string sender = message.substr(0, first);
            std::string meSend = "[" + USERNAME + "]: ";
            // std::cout << USERNAME << meSend << "\n";
            if (sender == meSend)
            {
                sender = "[Me]: ";
            }
            // get conversationIDString = conversationID
            std::string conversationIDString = message.substr(first + 1, second - (first + 1));

            int conversationIDReceive = atoi(conversationIDString.c_str());
            // get messageofConversation = message
            std::string messageofConversation = message.substr(second + 1);

            // Display receive message

            if (conversationID == conversationIDReceive && messageofConversation.length() >= 1)
            {
                client.eraseLines(1);
                std::string messageOfSender = sender + messageofConversation;
                std::cout << messageOfSender << std::endl;
                std::cout << "[me]: ";
                std::cout << std::flush;
            }
            else if (isdigit(sender.c_str()[0]) || sender.find("End conversation") != std::string::npos)
            {
                sleep(1);
                system("clear");
                client.getConversation(clientSocket);
                std::cout << "\033[1m\033[32mPress 1 for create new conversation\nPress 2 for choose conversation\nPress 3 for reload the conversation list\n";
                std::cout << "\033[1m\033[31mPress `Q` to log-out\n\033[39m";
                break;
            }

            memset(buffer, 0, sizeof(buffer));
        }
    }
}

// Function to send messages to the server
void sendMessage(int clientSocket, std::string idConversation)
{
    Client client;
    int convIDnumber = atoi(idConversation.c_str());
    flag_atomic = true;
    std::thread receiveThread(receiveMessage, clientSocket, convIDnumber);

    // Send message flow
    bool isExit = true;
    while (isExit)
    {
        std::cout << "[me]: ";
        std::string messageInput, messageSend;
        // client.eraseLines(1);
        std::getline(std::cin, messageInput);
        if (messageInput.length() >= 1 && messageInput == ":list")
        {
            messageSend = "QP$" + idConversation;
            send(clientSocket, messageSend.c_str(), messageSend.length(), 0);
            sleep(2);
        }
        else if (messageInput.length() >= 1 && messageInput == ":help")
        {
            client.listInstruction();
        }
        else if (messageInput.length() >= 1 && messageInput == ":add")
        {
            std::cout << "\033[1m\033[34mEnter the username of participant you want to add: \033[0m";
            std::string username;
            std::getline(std::cin, username);
            if (username == ":cancel")
            {
                continue;
            }
            while (!client.isValidUsername(username))
            {
                std::cerr << "\033[1m\033[33mUsername contains only alphanumeric form 5 to 20 characters. \nRe-enter username: \033[0m";
                std::getline(std::cin, username);
                if (username == ":cancel")
                    continue;
            }
            messageSend = "AN$" + idConversation + "$" + USERNAME + "$" + username;
            send(clientSocket, messageSend.c_str(), messageSend.length(), 0);
            sleep(2);
        }
        else if (messageInput.length() >= 1 && messageInput == ":delete")
        {
            std::cout << "\033[1m\033[34mEnter the username of participant you want to delete: \033[0m";
            std::string username;
            std::getline(std::cin, username);
            if (username == ":cancel")
                continue;

            while (!client.isValidUsername(username))
            {
                std::cerr << "\033[1m\033[33mUsername contains only alphanumeric form 5 to 20 characters. \nRe-enter username: \033[0m";
                std::getline(std::cin, username);
                if (username == ":cancel")
                    continue;
            }
            messageSend = "DE$" + idConversation + "$" + USERNAME + "$" + username;
            send(clientSocket, messageSend.c_str(), messageSend.length(), 0);
            sleep(2);
        }
        else if (messageInput.length() >= 1 && messageInput != ":exit")
        {
            messageSend = "M$" + idConversation + "$" + messageInput;
            send(clientSocket, messageSend.c_str(), messageSend.length(), 0);
        }
        else if (messageInput == ":exit")
        {
            system("clear");
            flag_atomic = false;
            if (receiveThread.joinable())
            {
                receiveThread.detach();
            }
            break;
        }
    }
}

bool Client::reconnectToServer(int &clientSocket)
{
    std::cout << "\n\033[1m\033[33mReconnecting...\033[0m\n";
    unsigned int wait = 0;
    bool checkIfConnect, reconnectFail = true;
    this->closeClientSocket(clientSocket);
    clientSocket = this->createClientSocket();
    while (wait < 5)
    {
        sleep(2);
        checkIfConnect = this->connectToServer(clientSocket, ipAddress, port);
        if (checkIfConnect)
            break;
        wait++;
        if (wait == 5)
        {
            this->closeClientSocket(clientSocket);
            reconnectFail = false;
        }
    }

    if (staticClient.username != "" && reconnectFail)
    {
        char bufferRestore[1024];
        std::string loginMessage;
        // std::cout << "User: " << staticClient.username << "\n";
        // std::cout << "Pass: " << staticClient.password << "\n";
        loginMessage = 'L' + staticClient.username + '$' + this->hashPassword(staticClient.username, staticClient.password);
        std::cout << "\e[1m\033[32mConnected\033[0m\n";
        send(clientSocket, loginMessage.c_str(), loginMessage.length(), 0);
        recv(clientSocket, bufferRestore, sizeof(bufferRestore), 0);
        sleep(1);
    }
    else
    {
        std::cout << "\x1b[31m";
        std::cout << "Restore server fail\033[0m\n";
        exit(1);
    }
    std::cout << "\x1b[0m";
    return true;
}

bool Client::listInstruction()
{
    std::cout << "\033[1m\033[34m-----------------------------COMMAND---------------------------------" << std::endl;
    std::cout << std::setw(10) << std::left << ":exit"
              << " Get out from the current conversation." << std::endl;
    std::cout << std::setw(10) << std::left << ":list "
              << " Print a list of participants in the current conversation." << std::endl;
    std::cout << std::setw(10) << std::left << ":add "
              << " Add the user to the current conversation." << std::endl;
    std::cout << std::setw(10) << std::left << ":delete "
              << " Delete the user from the current conversation." << std::endl;
    std::cout << "---------------------------END COMMAND--------------------------------\033[0m" << std::endl;
    return true;
}
