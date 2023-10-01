#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <thread>
#include <regex>
#include "arpa/inet.h"
#include "./unistd.h"
#include <openssl/sha.h>
#include <iomanip>
#include <atomic>
#include <conio.h>
#include <ctype.h>
#include <conio.h>
#include <cstdlib>
#include <time.h>
#include <random>

std::atomic<bool> flag_atomic;
std::string USERNAME;

std::string ipAddress = "192.168.122.65", logoutMessage = "OUT";
int port = 30001;

struct clientStruct
{
    std::string username;
    std::string password;
    std::string email;
    std::string fullname;
};

struct conversation
{
    int ID;
    std::string name = "";
    int numberOfPartipants;
    std::vector<std::string> participants;
};

class Client
{
private:
    struct clientStruct clientInformation;
    struct conversation conv;

public:
    // Valid for Register and Login
    bool isValidEmail(const std::string &email);
    bool isValidUsername(const std::string &username);
    bool isValidPassword(const std::string &password);
    bool isValidNumber(const std::string &number);
    bool isValidFullname(const std::string &fullname);

    std::string enterUsername();
    std::string enterPassword();
    std::string enterEmail();
    std::string enterFullname();

    Client() = default;
    // Erase line
    void eraseLines(int count);
    // Function to create a client socket
    int createClientSocket();
    // Function to connect to the server
    bool connectToServer(int clientSocket, const std::string &ipAddress, int port);
    // Function to hash the password
    std::string hashPassword(std::string username, std::string password);
    // Function to get user input and register an account
    bool registerAccount(int clientSocket, std::string registerMessage);
    // Function to get user input and log in to the server
    int login(int clientSocket, std::string loginMessage);
    // Function to close the client socket
    void closeClientSocket(int clientSocket);
    // Function to create conversation
    void createConversation(int clientSocket);
    // Function to get conversation
    void getConversation(int clientSocket);
    // Function choose conversation
    std::string chooseConversation(int clientSocket);
    // Function to choose type of conversation
    std::string chooseTypeOfConversation();
    bool reconnectToServer(int &clientSocket);
    bool getMessageHistory(std::string message);
    bool listUser(int clientSocket);
    bool listInstruction();
};
// Function to receive and display messages from the server
void receiveMessage(int clientSocket, int conversationID);
// Function to send messages to the server
void sendMessage(int clientSocket, std::string idConversation);
#endif // CLIENT_CLIENT_H_
