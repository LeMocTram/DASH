#include "client.h"
#include "clientFunction.cpp"
#include <conio.h>

using namespace std;

int main()
{
    Client client;

    int clientSocket = client.createClientSocket();
    if (clientSocket == -1)
    {
        cerr << "\033[1m\033[31mFailed to create client socket.\033[0m\n";
        exit(1);
    }
    if (client.connectToServer(clientSocket, ipAddress, port) == false)
    {
        cerr << "\n\033[1m\033[31mConnection failed. Please wait\033[0m\n";
    }
    else
    {
        cout << "Connected to the server.\n";
    }

    system("clear");
    cout << "\033[1m\033[33m";
    cout << "        (\\__/)                                                             (\\__/)\n";
    cout << "        (o^-^)                                                             (o^-^)\n";
    cout << "      z(_(\")(\")                                                          z(_(\")(\")\n\033[0m";

    cout << "\033[1m\033[37m___             ___   ________    __         ______     _______      ___    ___    ________\n";
    cout << "\\  \\    ___    /  /  |   _____|  |  |       /  ____|   /  ___  \\    |   \\__/   |  |   _____|\n";
    cout << " \\  \\  /   \\  /  /   |  |____    |  |      /  /       /  /   \\  \\   |          |  |  |____\n";
    cout << "  \\  \\/  _  \\/  /    |   ____|   |  |     |  |       |  |     |  |  |  |\\__/|  |  |   ____|\n";
    cout << "   \\    / \\    /     |  |_____   |  |____  \\  \\____   \\  \\___/  /   |  |    |  |  |  |_____\n";
    cout << "    \\__/   \\__/      |________|  |_______|  \\______|   \\_______/    |__|    |__|  |________|\n";
    cout << "\n\e[1m"
         << "\033[32m      *************************** Press any keys to start! ***************************\033[39m"
         << "\e[0m";

    getch();

    while (true)
    {
        std::cout << "\033]0;"
                  << "DASH CHAT APPLICATION"
                  << "\007";
    label:
        system("clear");
        cout << "\e[1m\033[32mPress `L` to login if you had account \nPress `R` to create account.\n\033[91mPress `Q` to quit app.\n\033[39m\e[0m";
        int value = getch();

        while (value != 108 && value != 114 && value != 113)
        {
            cerr << "\033[1m\033[31mIncorrect.\033[0m\n\n";
            cout << "\e[1m\033[32mPress `L` to login if you had account \nPress `R` to create account.\n\033[91mPress `Q` to quit app.\n\033[39m\e[0m";

            value = getch();
        }
        if (value == 108)
        {
            system("clear");
            cout << "\e[1m\033[32m____________________________LOGIN____________________________\n";
            cout << "Command `:return` to back.\n\033[39m\e[0m\n";

            int state = true;
            int count = 3;
            do
            {
                count--;
                string loginMessage;
                loginMessage.clear();

                cout << "\e[1mUsername: \e[0m";
                string username = client.enterUsername();
                if (username == ":return")
                {
                    state = false;
                    break;
                }
                string password = client.enterPassword();
                if (password == ":return")
                {
                    state = false;
                    break;
                }

                staticClient.username.clear();
                staticClient.password.clear();
                staticClient.username = username;
                staticClient.password = password;
                loginMessage = 'L' + username + '$' + client.hashPassword(username, password);
                if (client.login(clientSocket, loginMessage) == true)
                {
                    cout << "Login success! \n";
                    USERNAME = username;
                    break;
                }
                else if (client.login(clientSocket, loginMessage) == false && count != 0)
                {
                    std ::cout << "\033[1m\033[33m\nThe number of login attempts remaining is " << count << ".\n";
                    std::cerr << "\033[1m\033[33mUsername or Password wrong!.\nPlease re-enter.\n\e[0m";
                }
                if (count == 0)
                {
                    std::cout << "\033[1m\033[31mYou have logged in incorrectly more than 3 times.\n\033[0m";
                    close(clientSocket);
                    exit(0);
                }
            } while (true);
            if (state == false)
            {
                goto label;
            }
        }
        else if (value == 114)
        {
            bool state = true;
            do
            {
                string registerMessage;
                registerMessage.clear();
                cout << "\e[1m\033[32m____________________________REGISTER____________________________\n";
                cout << "Command `:return` to back.\n\033[39m\e[0m\n";
                cout << "\e[1mUsername: \e[0m";
                string username = client.enterUsername();
                if (username == ":return")
                {
                    state = false;
                    break;
                }
                string password = client.enterPassword();
                if (password == ":return")
                {
                    state = false;
                    break;
                }
                string email = client.enterEmail();
                if (email == ":return")
                {
                    state = false;
                    break;
                }
                string fullname = client.enterFullname();
                if (fullname == ":return")
                {
                    state = false;
                    break;
                }
                registerMessage = 'R' + username + '$' + client.hashPassword(username, password) + '$' + email + '$' + fullname;

                if (client.registerAccount(clientSocket, registerMessage) == true)
                {
                    cout << "Register success! \n";
                    USERNAME = username;
                    break;
                }
                else
                {
                    cerr << "\033[1m\033[31mUsername already exists. Please choose a different one.\033[0m\n\n";
                }
            } while (true);

            if (state == false)
            {
                goto label;
            }
        }
        else
        {
            client.closeClientSocket(clientSocket);
            break;
        }
        while (true)
        {
            system("clear");
            client.getConversation(clientSocket);
            cout << "\033[1m\033[32mPress 1 for create new conversation\nPress 2 for choose conversation\n";
            cout << "Press 3 for reload the conversation list\n";
            cout << "\033[1m\033[31mPress `Q` to log-out\n\033[39m";

            int value = getch();
            while (value != 49 && value != 50 && value != 51 && value != 113)
            {
                cout << "\033[1m\033[31mIncorrect.\n\033[39m";
                value = getch();
            }

            if (value == 49)
            {
                client.listUser(clientSocket);
                client.createConversation(clientSocket);
            }
            else if (value == 50)
            {
                string idConversation;
                idConversation = client.chooseConversation(clientSocket);
                sendMessage(clientSocket, idConversation);
            }
            else if (value == 51)
            {
                client.getConversation(clientSocket);
            }
            else if (value == 113)
            {
                send(clientSocket, logoutMessage.c_str(), logoutMessage.length(), 0);
                staticClient.username.clear();
                staticClient.password.clear();
                break;
            }
        }
    }

    return 0;
}
