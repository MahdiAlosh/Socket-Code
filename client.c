#include <winsock2.h>
#include <stdio.h>

int main()
{
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(12345);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Connection failed.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    // Send data to the server
    while (1)
    {
        char message[1024];
        printf("Enter a message to send to the server: ");
        fgets(message, sizeof(message), stdin);

        // Remove the newline character if present
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n')
        {
            message[len - 1] = '\0';
        }

        // Check if the user wants to quit
        if (message[0] == 'q' && (message[1] == '\0' || message[1] == '\n'))
        {
            printf("Quitting...\n");
            break;
        }

        if (strlen(message) == 0)
        {
            printf("empty message. Nothing sent!!\nto exit enter 'q':\n");
        }
        else
        {
            // Send data to the server
            send(clientSocket, message, strlen(message), 0);
            printf("Sent: %s\n", message);
        }
    }
    // Close the socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
