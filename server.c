//? IPv4

// #include <winsock2.h>
// #include <stdio.h>

// // TODO 1. Port selber definieren #define PORT 7923
// #define PORT 12345

// int main(void)
// {
//     WSADATA wsaData;
//     /**
//      * Die WSADATA-Struktur wird verwendet,
//      * um Informationen über die Winsock-Implementierung zu speichern.
//      **/

//     SOCKET serverSocket, clientSocket;
//     struct sockaddr_in serverAddr, clientAddr;
//     int clientAddrLen = sizeof(clientAddr);

//     //! Initialize Winsock
//     // WSAStartup ist eine Funktion aus der Winsock-Bibliothek,
//     // die erforderlich ist, um die Socket-Funktionalität unter Windows zu verwenden.
//     // MAKEWORD(2, 2) gibt an, dass die Anwendung die Version 2.2 der Winsock-Spezifikation verwenden möchte.
//     // Wenn WSAStartup erfolgreich ist, gibt sie 0 zurück. Andernfalls wird eine Fehlermeldung ausgegeben,
//     // und das Programm wird mit return -1; beendet.
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//     {
//         fprintf(stderr, "WSAStartup failed.\n");
//         /**
//          * stderr:
//          * Falls die Initialisierung der Winsock-Bibliothek nicht erfolgreich war,
//          * wird eine Fehlermeldung auf den Standardfehlerausgabestrom (stderr) geschrieben.
//          * Diese Meldung informiert darüber, dass die Initialisierung des Sockets nicht erfolgreich war.
//          **/
//         return -1;
//     }

//     //! Create a socket
//     /**
//      * AF_INET: gibt an, dass es sich um einen IPv4-Socket handelt.
//      * SOCK_STREAM: gibt an, dass es sich um einen TCP-Socket handelt.
//      * Das dritte Argument (0): steht normalerweise für das Protokoll,
//      * und hier wird der Standardwert 0 verwendet, was bedeutet,
//      * dass das System das Protokoll basierend auf den vorherigen Parametern auswählen soll.
//      * Da sich es hier um TCP_Socket handelt, bleibt in der regel 0
//      **/
//     serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == INVALID_SOCKET)
//     {
//         fprintf(stderr, "Socket creation failed.\n");
//         WSACleanup();
//         return -1; // 1
//     }

//     //! Set up server address structure
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     serverAddr.sin_port = htons(PORT);

//     //! Bind the socket
//     if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//     {
//         fprintf(stderr, "Bind failed.\n");
//         closesocket(serverSocket);
//         WSACleanup();
//         return -1; // 1
//     }

//     //! Listen for incoming connections
//     listen(serverSocket, 5); // listen(int sd, int backlog)
//     // backlog: Die Länge der Warteschlange für eingehende Verbindungsanforderungen.
//     // backlog-Parameter gibt an, wie viele Verbindungen in dieser Warteschlange gehalten werden können
//     printf("Server listening on port %d ...\n", PORT);

//     //! Accept a connection
//     clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
//     if (clientSocket == INVALID_SOCKET)
//     {
//         fprintf(stderr, "Accept failed.\n");
//         closesocket(serverSocket);
//         WSACleanup();
//         return -1;
//     }

//     printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

//     //! Receive and print data
//     while (1)
//     {
//         char buffer[1024];
//         int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
//         if (bytesRead <= 0)
//         {
//             printf("No data received from the client or client connection has been terminated!!\n");
//             break;
//         }
//         else
//         {
//             printf("Received: %.*s\n", bytesRead, buffer);
//         }
//     }

//     //! Close sockets
//     closesocket(clientSocket);
//     closesocket(serverSocket);
//     printf("sockets have been closed.\n");
//     WSACleanup();

//     return 0;
// }

//? IPv6
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT "12345" // Define the port as a string
// Annahme: IP-Adresse des Servers ist "127.0.0.1"
const char *serverIPAddress;

int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct addrinfo serverAddr, clientAddr, *result = NULL;
    socklen_t clientAddrLen = sizeof(clientAddr);

    //! Initialize Winsock
    // WSAStartup ist eine Funktion aus der Winsock-Bibliothek,
    // die erforderlich ist, um die Socket-Funktionalität unter Windows zu verwenden.
    // MAKEWORD(2, 2) gibt an, dass die Anwendung die Version 2.2 der Winsock-Spezifikation verwenden möchte.
    // Wenn WSAStartup erfolgreich ist, gibt sie 0 zurück. Andernfalls wird eine Fehlermeldung ausgegeben,
    // und das Programm wird mit return -1; beendet.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        return -1;
    }

    // stderr:
    // Falls die Initialisierung der Winsock-Bibliothek nicht erfolgreich war,
    // wird eine Fehlermeldung auf den Standardfehlerausgabestrom (stderr) geschrieben.
    // Diese Meldung informiert darüber, dass die Initialisierung des Sockets nicht erfolgreich war.

    //! Set up serverAddr structure
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.ai_family = AF_INET6;      // Use IPv6
    serverAddr.ai_socktype = SOCK_STREAM; // Use TCP
    serverAddr.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    // serverAddr.ai_flags = AI_PASSIVE;  // For the server, specify AI_PASSIVE to get the local address

    //! Resolve the server address and port
    if (getaddrinfo(serverIPAddress, PORT, &serverAddr, &result) != 0)
    {
        fprintf(stderr, "getaddrinfo failed.\n");
        WSACleanup();
        return -1;
    }

    //! Create a socket
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serverSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed.\n");
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    //! Bind the socket
    if (bind(serverSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        fprintf(stderr, "Bind failed.\n");
        closesocket(serverSocket);
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    //! Listen for incoming connections
    listen(serverSocket, 5); // listen(int sd, int backlog)
    // backlog: Die Länge der Warteschlange für eingehende Verbindungsanforderungen.
    // backlog-Parameter gibt an, wie viele Verbindungen in dieser Warteschlange gehalten werden können
    printf("Server listening on port %s...\n", PORT);

    //! Accept a connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Accept failed.\n");
        closesocket(serverSocket);
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    //! Print client connection information
    char clientAddrStr[INET6_ADDRSTRLEN];
    getpeername(clientSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (getnameinfo((struct sockaddr *)&clientAddr, clientAddrLen, clientAddrStr, sizeof(clientAddrLen), NULL, 0, NI_NUMERICHOST) == 0)
    {
        printf("Connection accepted from %s: %d\n", clientAddrStr, ntohs(((struct sockaddr_in6 *)&clientAddr)->sin6_port));
    }

    /**
     * getnameinfo: Diese Funktion übersetzt Netzwerkadressen in lesbare Form.
     * Hier wird sie verwendet, um die IP-Adresse des Clients und den Port in lesbarer Form zu extrahieren.
     * Die Parameter sind:
     * (struct sockaddr *)&clientAddr: Ein Zeiger auf die Adresse, die übersetzt werden soll.
     * clientAddrLen: Die Länge der Adresse.
     * clientAddrStr: Ein Zeiger auf den Puffer, in dem die resultierende IP-Adresse gespeichert wird.
     * sizeof(clientAddrLen): Die Größe des Puffers.
     *
     * ntohs(((struct sockaddr_in6 *)&clientAddr)->sin6_port):
     * Hier wird der Port extrahiert und von Netzwerkbyte-Reihenfolge (Big-Endian) in Hostbyte-Reihenfolge (systemabhängig) umgewandelt.
     * Dies ist notwendig, da Netzwerkprotokolle oft in Big-Endian-Reihenfolge arbeiten, während die Hostmaschine möglicherweise in Little-Endian-Reihenfolge arbeitet.
     */

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
            printf("s%i > %.*s\n", clientSocket, bytesRead, buffer);
        }
    }

    //! Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    freeaddrinfo(result);
    WSACleanup();

    return 0;
}
