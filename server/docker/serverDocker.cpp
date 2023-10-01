// #include <iostream>
// #include <cstring>
// #include <arpa/inet.h>
// #include <unistd.h>
// ABCDsdfdsf
// #include "serverFunction.h"
#include "dockerFunction.cpp"
#include "../../database/docker/dbDocker.cpp"

//////////////Main function///////////////////
int main()
{
    // create socket for server
    int maxFd;
    sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    fd_set fdSocketSet; // set of fd

    ServerFunction Server;
    // Server.setServerSocket(Server.createServerSocket());
    Client newClient = {"", 0, -1};
    memset(&newClient.address, 0, sizeof(struct sockaddr_in));
    // intitalize list of existingsocket and client array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        Server.setClient(i, &newClient);

        Server.setExistingSocket(i, 0);
    }

    // call function setSeverPort

    if (Server.setServerAddress() != 0)
    {
        std::cerr << "Create server address failed.\n";
        exit(EXIT_FAILURE);
    }

    // call function initServerSocket to Initialize server + bind + listen
    if (Server.initServerSocket() != 0)
    {
        std::cerr << "Initialized server failed.\n";
        exit(EXIT_FAILURE);
    }

    // new connection elements

    while (true)
    {
        FD_ZERO(&fdSocketSet);
        FD_SET(Server.getServerSocket(), &fdSocketSet);

        maxFd = Server.getServerSocket();

        // Set file desciptor if new client enter
        Server.setNewFileDescriptor(&fdSocketSet, maxFd);

        // Select function
        int activity = select(maxFd + 1, &fdSocketSet, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        { // error select
            std::cout << std::endl
                      << "Select error" << std::endl;
        }
        // Check if there is new connection
        if (FD_ISSET(Server.getServerSocket(), &fdSocketSet))
        {
            // Create new socket
            Server.acceptNewConnection();
        }

        Server.checkOperationOnOtherSocket(&fdSocketSet); // check if there is any operation on other socket
    }

    close(Server.getServerSocket()); // close Server

    return 0;
}
