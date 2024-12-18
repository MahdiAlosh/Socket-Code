#define _CRT_SECURE_NO_WARNINGS

#include "packet.h"
#include <stdio.h>

#include <winsock2.h> 
//Die Bibliothek winsock2.h ist eine Header-Datei in der Windows-API, die Funktionen und Strukturen für die Netzwerkprogrammierung in Windows bereitstellt. 
//Kurz gesagt, sie ermöglicht die Verwendung von Winsock, einer API für die Implementierung von Netzwerkdiensten in Windows-Anwendungen. 
//Mit winsock2.h kann man TCP/IP-Verbindungen erstellen, Sockets verwenden, Daten übertragen und andere Netzwerkaufgaben in ihren Windows-Anwendungen durchführen.

#include <ws2tcpip.h>
//Die Header-Datei ws2tcpip.h in der Windows-API stellt Funktionen und Definitionen für erweiterte Netzwerkfunktionen bereit, die über die Grundlagen von Winsock hinausgehen. 
//Insbesondere bietet sie Funktionen für die Verwendung von IPv6 und erweiterte Funktionen für die Arbeit mit TCP/IP-Protokollen. 
//Kurz gesagt, ws2tcpip.h ist eine Erweiterung von winsock2.h und ermöglicht den Zugriff auf fortgeschrittene Netzwerkfunktionen in Windows-Anwendungen.

#include <conio.h>
//Die Header-Datei conio.h (Console Input/Output) ist eine C-Standard-Header-Datei, die Funktionen für die Konsoleneingabe und -ausgabe bereitstellt. 
//-> ermöglicht das Lesen von Tastatureingaben und das Ändern von Konsolenattributen in C-Programmen, insbesondere auf Plattformen wie DOS und Windows.

//Enthält: "getch()" (Zum lesen eines einzelnen Zeichens) und "kbhit()" (Zum überprüfen, ob auf der Taste etwas gedrückt wurde)

//Definition der maximalen Messagelänge und der größes des Speicherbuffers
#define MAX_MESSAGE_LENGTH 1024
#define MAX_BUFFER_LENGTH 1024

//Die Funktion f_wsadata initialisiert die Winsock-Bibliothek in einer Windows-Anwendung. 
//Wenn die Initialisierung fehlschlägt, wird eine Fehlermeldung ausgegeben, und das Programm wird mit einem Fehlercode beendet. 
//Winsock wird für Netzwerkoperationen in Windows-Anwendungen verwendet.
void f_wsadata(){
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        exit(EXIT_FAILURE);
    }
}

//Die Funktion f_clientSocket erstellt einen Socket für einen Client in einer Windows-Anwendung. 
//Falls die Socket-Erstellung fehlschlägt, gibt sie eine Fehlermeldung aus, gibt Ressourcen frei, deinitialisiert die Winsock-Bibliothek und beendet das Programm mit einem Fehlercode.
//Bei einem Erfolg, wird dies ebenfalls an den Nutzer zurückgegeben.
SOCKET f_clientSocket(struct addrinfo* result){
    SOCKET fclientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (fclientSocket == INVALID_SOCKET) {
        perror("Client socket creation failed");
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Client socket created successfully...\n");
    return fclientSocket;
}

//Die Funktion f_connToServer stellt eine Verbindung zu einem Server über einen angegebenen Client-Socket in einer Windows-Anwendung her. 
//Bei einem Verbindungsfehler gibt sie eine Fehlermeldung aus, schließt den Socket, gibt Ressourcen frei, deinitialisiert die Winsock-Bibliothek und beendet das Programm mit einem Fehlercode. 
//Bei erfolgreicher Verbindung gibt sie eine Erfolgsmeldung aus.
void f_connToServer(SOCKET clientSocket, struct addrinfo* result){
    if (connect(clientSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR) {
        fprintf(stderr, "Connection to server failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Connected to server successfully...\n");
}
//Die Funktion f_getaddrinfo ruft die Funktion getaddrinfo auf, um Netzwerkadressinformationen für eine gegebene IP-Adresse und Portnummer zu erhalten. 
//Bei einem Fehler gibt sie eine Fehlermeldung aus, deinitialisiert die Winsock-Bibliothek und beendet das Programm mit einem Fehlercode. 
//Andernfalls gibt sie die erhaltenen Adressinformationen zurück.
struct addrinfo* f_getaddrinfo(const char* IP,const char* PORT, struct addrinfo hints, struct addrinfo* result){
    if (getaddrinfo(IP, PORT, &hints, &result) != 0) {
        perror("getaddrinfo failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return result;
}

//Die Funktion f_delete löscht die letzten index + 5 Zeichen auf der Konsole durch Rückgängigmachen (\b \b).
void f_delete(int index){
    for(int a=1; a <= index+5; a++){
        printf("\b \b");
    }
}

//Die Funktion f_chat ermöglicht einen einfachen Chat über einen angegebenen Client-Socket in einer Windows-Anwendung. 
//Die Funktion überwacht sowohl eingehende Nachrichten vom Server als auch Benutzereingaben über die Konsole. 
//Bei eingehenden Nachrichten werden sie auf der Konsole angezeigt. 
//Der Benutzer kann Nachrichten eingeben und sie an den Server senden. Die Schleife wird durch den Befehl "Quit" beendet.
void f_chat(SOCKET clientSocket, char myid[]){
    fd_set read_fds;
    struct timeval timeout;
    char message[MAX_MESSAGE_LENGTH];
    char buf[MAX_BUFFER_LENGTH];

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(clientSocket, &read_fds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int ready = select(0, &read_fds, NULL, NULL, &timeout);

        if (ready == SOCKET_ERROR) {
            perror("select");
            break;
        }

        if (ready > 0 && FD_ISSET(clientSocket, &read_fds)) {
            packet myPacket;

            int bytesReceived = recv(clientSocket, (char*)&myPacket, sizeof(myPacket), 0);

            if (bytesReceived > 0) {
                printf("%s: %s\n", myPacket.sNum, myPacket.text);
            } else if (bytesReceived == 0) {
                printf("Connection closed by the server\n");
                break;
            } else {
                //perror("recv");
                printf("Connection interrupted by the server\n");
                break;
            }
        }

        if (_kbhit()) {
            printf("You: ");

            int index = 0;
            char ch;

            while (1) { //read während in Input-mode
                FD_ZERO(&read_fds);
                FD_SET(clientSocket, &read_fds);

                timeout.tv_sec = 0;
                timeout.tv_usec = 0;

                int ready_r = select(0, &read_fds, NULL, NULL, &timeout);

                if (ready_r == SOCKET_ERROR) {
                    perror("select");
                    break;
                }

                if (ready_r > 0 && FD_ISSET(clientSocket, &read_fds)) {
                    packet myPacket;

                    int bytesReceived = recv(clientSocket, (char*)&myPacket, sizeof(myPacket), 0);

                    if (bytesReceived > 0) {
                        message[index] = '\0';
                        //printf("\r");
                        f_delete(index);
                        printf("%s: %s\n", myPacket.sNum, myPacket.text);
                        printf("You: %s", message);
                    } else if (bytesReceived == 0) {
                        printf("Connection closed by the server\n");
                        break;
                    } else {
                        //perror("recv");
                        printf("Connection interrupted by the server\n");
                        break;
                    }
                }

                if (_kbhit()) {
                    ch = _getch();
                    if (ch == 8 && index > 0) {
                        printf("\b \b");
                        index--;
                    } else if (ch == 8 && index == 0) {
                        index = 0;
                    } else {
                        message[index++] = ch;
                        printf("%c", ch);
                    }

                    if (ch == '\r' || ch == '\n' || index >= sizeof(message) - 1) {
                        break;
                    }
                }
            }

            message[index] = '\0';

            if(strcmp(message, "Quit\r") == 0){
                break;
            }

            packet myPacket;

            strcpy(myPacket.sNum, myid);
            myPacket.sNum[sizeof(myPacket.sNum) - 1] = '\0';

            strcpy(myPacket.text, message);
            myPacket.text[sizeof(myPacket.text) - 1] = '\0';

            send(clientSocket, (void*)&myPacket, sizeof(myPacket), 0);

            printf("\n");
        }
    }
}
//Die Funktion f_close schließt eine Socket-Verbindung in einer Windows-Anwendung. 
//Sie gibt eine Abschlussmeldung aus, schließt den Socket und deinitialisiert die Winsock-Bibliothek.
void f_close(SOCKET clientSocket){
    printf("Closing Connection...\n");
    closesocket(clientSocket);
    WSACleanup();
}

//Überprüft, ob ausreichende Kommandozeilenargumente vorhanden sind. 
//Falls nicht, wird eine Benutzungsnachricht ausgegeben, und das Programm wird mit einem Fehlercode beendet.
int main(int argc,char* argv[]) {

//Entnimmt die IPv6-Adresse, den Port und die S-Nummer aus den Kommandozeilenargumenten.
    if(argc < 4){
        printf("Usage: %s <IPv6- Adresse> <Port> <S-Nummer>",argv[0]);
        exit(1);
    }

    const char* IP = argv[1];
    const char* PORT = argv[2];
    char myid[10];
    strncpy(myid, argv[3], sizeof(myid) - 1);
    
    //Initialisiert die Winsock-Bibliothek (f_wsadata).
    f_wsadata();
    
    struct addrinfo* result = NULL;
    struct addrinfo hints;

//Konfiguriert die Adressinformationen für den Server (hints und f_getaddrinfo).
//Erstellt einen Client-Socket und stellt eine Verbindung zum Server her (f_clientSocket und f_connToServer).
    SOCKET clientSocket;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = f_getaddrinfo(IP, PORT, hints, result);
    clientSocket = f_clientSocket(result);

    f_connToServer(clientSocket, result);
    freeaddrinfo(result);
//Startet den Chat zwischen dem Client und dem Server (f_chat).
    f_chat(clientSocket, myid);
//Schließt die Socket-Verbindung und deinitialisiert die Winsock-Bibliothek (f_close).
    f_close(clientSocket);

//Beendet das Programm mit einem Erfolgscode.
    return 0;
}
