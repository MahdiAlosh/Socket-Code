#include <winsock2.h>
#include <stdio.h>

int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Bind failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    listen(serverSocket, 1);
    printf("Server listening on port 12345...\n");

    // Accept a connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Accept failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Receive and print data
    while (1)
    {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            printf("No data received from the client or client connection has been terminated!!\n");
            break;
        }
        else
        {
            printf("Received: %.*s\n", bytesRead, buffer);
        }
    }

    // Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    printf("sockets have been closed.\n");
    WSACleanup();

    return 0;
}
