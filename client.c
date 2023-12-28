//? IPv4

// #include <winsock2.h>
// #include <stdio.h>

// // TODO 1. Port selber definieren #define PORT 7923
// // TODO 2. Server selber defi. #define SERVER "192.168.0.2"
// #define PORT 12345         // 7923
// #define SERVER "127.0.0.1" //"192.168.0.2"

// int main(void)
// {
//     WSADATA wsaData;
//     SOCKET clientSocket;
//     struct sockaddr_in serverAddr;

//     //! Initialize Winsock
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//     {
//         fprintf(stderr, "WSAStartup failed.\n");
//         return -1; // 1
//     }

//     //! Set up server address structure
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = inet_addr(SERVER);
//     serverAddr.sin_port = htons(PORT);

//     //! Create a socket
//     clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (clientSocket == INVALID_SOCKET)
//     {
//         fprintf(stderr, "Socket creation failed.\n");
//         WSACleanup();
//         return -1; // 1
//     }

//     //! Connect to the server
//     if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//     {
//         fprintf(stderr, "Connection failed.\n");
//         closesocket(clientSocket);
//         WSACleanup();
//         return -1; // 1
//     }

//     printf("Connected to server.\n");

//     //! Send data to the server
//     while (1)
//     {
//         char message[1024];
//         printf("Enter a message to send to the server: ");
//         fgets(message, sizeof(message), stdin);

//         //! Remove the newline character if present
//         size_t len = strlen(message);
//         if (len > 0 && message[len - 1] == '\n')
//         {
//             message[len - 1] = '\0';
//         }

//         //! Check if the user wants to quit
//         if (message[0] == 'q' && (message[1] == '\0' || message[1] == '\n'))
//         {
//             printf("Quitting...\n");
//             break;
//         }

//         if (strlen(message) == 0)
//         {
//             printf("empty message. Nothing sent!!\nto exit enter 'q':\n");
//         }
//         else
//         {
//             //! Send data to the server
//             send(clientSocket, message, strlen(message), 0);
//             printf("Sent: %s\n", message);
//         }
//     }

//     //! Close the socket
//     closesocket(clientSocket);
//     WSACleanup();

//     return 0;
// }

//? IPv6
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// TODO 1. Port selber definieren #define PORT 7923
// TODO 2. Server selber defi. Home IP_Adress #define SERVER "192.168.178.1"
#define PORT "12345"
#define SERVER "::1"

int main(void)
{
    WSADATA wsaData;
    SOCKET clientSocket;
    struct addrinfo serverAddr, *result = NULL;

    //! Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        return -1; // 1
    }

    //! Set up hints structure
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.ai_family = AF_INET6;      // Use IPv6
    serverAddr.ai_socktype = SOCK_STREAM; // Use TCP
    serverAddr.ai_protocol = IPPROTO_TCP; // Use TCP protocol

    //! Resolve the server address and port
    if (getaddrinfo(SERVER, PORT, &serverAddr, &result) != 0)
    {
        fprintf(stderr, "getaddrinfo failed.\n");
        WSACleanup();
        return -1;
    }

    //! Create a socket
    clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed.\n");
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    //! Connect to the server
    if (connect(clientSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        fprintf(stderr, "Connection failed.\n");
        closesocket(clientSocket);
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    printf("Connected to server.\n");

    //! Send data to the server
    while (1)
    {
        char message[1024];
        printf("Enter a message to send to the server: ");
        fgets(message, sizeof(message), stdin); // Eigene Nachricht auf Console eingeben.

        //! Remove the newline character if present
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n')
        {
            message[len - 1] = '\0';
        }

        //! Check if the user wants to quit
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
            //! Send data to the server
            send(clientSocket, message, strlen(message), 0);
            printf("Sent: %s\n", message);
        }
    }

    //! Close the socket
    closesocket(clientSocket);
    freeaddrinfo(result);
    WSACleanup();

    return 0;
}
