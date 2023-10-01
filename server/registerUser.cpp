#include <iostream>
#include <string.h>

using namespace std;

class new_client
{
private:
    string username;
    string password;
    string email;
    string full_name;
    unsigned int status;

public:
    new_client()
    {
        cout << "------------- Creating new Client --------" << endl;
    }
    void register_client();
    void display_client();
};

// new_client* register_client();

void new_client::register_client()
{
    // new_client temp;
    cout << "Enter username: ";
    getline(cin, username);
    cout << endl;

    cout << "Enter password: ";
    getline(cin, password);
    cout << endl;

    cout << "Enter email: ";
    getline(cin, email);
    cout << endl;

    cout << "Enter full name: ";
    getline(cin, full_name);
    cout << endl;

    cout << "Enter status: ";
    cin >> status;
    cout << endl;
}
void new_client::display_client()
{
    cout << "username: " << username << endl;
    cout << "password: " << password << endl;
    cout << "email: " << email << endl;
    cout << "full name: " << full_name << endl;
    cout << "status: " << status << endl;
}

int main(void)
{
    new_client user1;
    user1.register_client();
    user1.display_client();
    return 0;
}
