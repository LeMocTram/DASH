#include "server/serverFunction.cpp"
#include "server/serverFunction.h"

#include "database/db.cpp"
#include "database/db.h"

#include "client/client.h"
#include "client/clientFunction.cpp"

#include <limits.h>
#include <gtest/gtest.h>

using namespace std;

// Test case for create socket
TEST(CreateServerSocketTest, CreateServerSocketSuccess)
{

    ServerFunction server;
    int serverSocket = server.createServerSocket();
    EXPECT_GT(serverSocket, 2);
}

// Test case for authenticate user: authenticate success
TEST(AuthenticateUserTest, AuthenticateUserSuccess)
{
    ServerFunction server;
    std::string testString = "";
    testString.append("Ltrung$01843d6aefff3bb388d29946b1a71a451720cfbbacef73fadaa9e69fa621a1caac783e15c257ade065c1440dd1587fffe5c563aa68d0d412db0a66121e101c60");
    char *testStringChar = new char[testString.length() + 1];
    std::strcpy(testStringChar, testString.c_str());
    std::string username = server.processLogin(1, testStringChar);
    EXPECT_EQ("trung", username);
}

// Test case for authenticate user: authenticate fail
TEST(AuthenticateUserTest, AuthenticateUserFail)
{
    ServerFunction server;
    std::string testString = "";
    testString.append("Ltrung123456$01843d6aefff3bb388d29946b1a71a451720cfbbacef73fadaa9e69fa621a1caac783e15c257ade065c1440dd1587fffe5c563aa68d0d412db0a66121e101c60");
    char *testStringChar = new char[testString.length() + 1];
    std::strcpy(testStringChar, testString.c_str());
    std::string username;
    username = server.processLogin(1, testStringChar);
    EXPECT_NE("trung123456", username);
}

// Test case for list participate handler: conversation exist
TEST(ListConversationTest, ListConversationExist)
{
    ServerFunction server;
    std::string listParticipantHandler = server.participantsListHandler("1");
    std::string listParticipantsString = "Qtram$trung$";
    std::cout << listParticipantHandler << "\n";
    EXPECT_NE(listParticipantHandler, listParticipantsString);
}

// Test case for register: use exist => return ""
TEST(RegisterTest, RegisterUserExist)
{
    ServerFunction server;
    std::string registerResultString = server.processRegister(4, "Rtrung$trung123$trung@gmail.com$Trung");
    EXPECT_EQ("", registerResultString);
}

// Test case for valid username
TEST(Testfunction, testValidUsername)

{
    Client client;
    std::string username = "tram";
    EXPECT_FALSE(client.isValidUsername(username));
}

// Test case for valid password
TEST(Testfunction, testValidPassword)

{
    Client client;
    std::string password = "tram";
    EXPECT_FALSE(client.isValidPassword(password));
}

// Test case for valid email
TEST(Testfunction, testValidEmail)

{
    Client client;
    std::string email = "tram@gmail.com";
    EXPECT_TRUE(client.isValidEmail(email));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}
