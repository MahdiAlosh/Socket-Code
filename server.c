#define _CRT_SECURE_NO_WARNINGS


#include "packet.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>


#define IP "::1"
#define MAX_MESSAGE_LENGTH 1024
#define MAX_BUFFER_LENGTH 1024

//! Windows Socket initialisieren
void f_wsadata(){
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // Winsock-Version 2.2 verwenden beim == 0 erfolgreich initialisieret!
        perror("Failed to initialize Winsock");      // sonst !=0 Fehler beim initialisieren
        // Diese Funktion gibt eine Fehlermeldung auf der Konsole aus, die besagt,
        // dass das Initialisieren des Sockets fehlgeschlagen ist. Die perror-Funktion verwendet den globalen Fehlercode,
        // um eine passende Fehlermeldung anzuzeigen.
        exit(EXIT_FAILURE);
    }
}

//! Server-Socket für die Kommunikation über IPv6 erstellen
// Hier wird ein Socket mit den angegebenen Parametern erstellt.
// AF_INET6 gibt an, dass IPv6 verwendet werden soll,
// SOCK_STREAM steht für einen TCP-Socket, und IPPROTO_TCP ist das Protokoll, das für diesen Socket verwendet wird.
SOCKET f_serverSocket(){
    SOCKET fserverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (fserverSocket == INVALID_SOCKET) { // Fehler beim erstellen den Socket
        perror("Socket creation failed");
        WSACleanup();
        // Die Funktion WSACleanup wird aufgerufen, um Winsock zu bereinigen. Dies ist notwendig,
        // um Ressourcen freizugeben, die während der Winsock-Initialisierung reserviert wurden.
        exit(EXIT_FAILURE);
    }
    printf("Server socket created successfully...\n");
    return fserverSocket;
}

//! die Netzwerkadresseinformationen für den Server Vorbereiten.
// Diese Informationen können dann verwendet werden, um den Server-Socket zu binden
// hint:    dient als Hinweis oder Anforderung für das gewünschte Adressformat.
// result:  Zeiger auf eine Struktur vom Typ addrinfo. Diese Struktur wird von getaddrinfo verwendet,
// um die erhaltenen Netzwerkadresseinformationen zu speichern.
struct addrinfo* f_getaddrinfo(const char* PORT,SOCKET serverSocket, struct addrinfo* result, struct addrinfo hints){
    if (getaddrinfo(IP, PORT, &hints, &result) != 0) {
        perror("getaddrinfo failed");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return result;
    // Wenn die Funktion erfolgreich war, wird der Zeiger auf die addrinfo-Struktur zurückgegeben.
    // Diese Struktur enthält Informationen über die Netzwerkadresse des Servers und wird dann für das Binden des Server-Sockets verwendet.
}
//!  der Server-Socket an eine bestimmte Adresse und einen bestimmten Port binden.
void f_bind(const char* PORT, SOCKET serverSocket, struct addrinfo* result){
    if (bind(serverSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR) {
        // Die Adresse und die Länge der Adresse werden aus der result-Struktur abgerufen.
        perror("Bind failed");
        closesocket(serverSocket);
        freeaddrinfo(result); // wird aufgerufen, um den Speicher freizugeben, der für die addrinfo-Struktur (aus der getaddrinfo-Funktion) reserviert wurde.
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Bind to Port Number %s \n", PORT);
}
//! der Server-Socket in den Zustand des Zuhörens (Listening) versetzen, sodass er eingehende Verbindungsanfragen akzeptieren kann.
void f_listen(SOCKET serverSocket){
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        // SOMAXCONN gibt an, dass der Socket eine maximale Warteschlangengröße für eingehende Verbindungen verwenden soll.
        perror("Listen failed");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");
}
//! eine eingehende Verbindung vom Server-Socket akzeptieren und einen neuen Socket für die Kommunikation mit dem Client erstellen.
// Hier wird die Funktion accept aufgerufen, um eine eingehende Verbindung vom Server-Socket zu akzeptieren.
// Die Funktion gibt einen neuen Socket zurück (fclientSocket), der für die Kommunikation mit dem Client verwendet wird.
// Die Informationen über den Client, wie z.B. die IP-Adresse und der Port, werden in der clientAddr-Struktur gespeichert.
SOCKET f_acc(SOCKET serverSocket){
    struct sockaddr_in6 clientAddr;
    int addr_size = sizeof(clientAddr);
    SOCKET fclientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addr_size);
    if (fclientSocket == INVALID_SOCKET) {
        perror("Accept failed");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Connected\n");

    return fclientSocket;
    // Der erstellte Client-Socket wird zurückgegeben, sodass er für die Kommunikation mit dem Client verwendet werden kann.
}
//! eine bestimmte Anzahl von Zeichen auf der Konsole löschen. 
// Sie verwendet den Backspace-Steuerzeichen (\b), um den Cursor um ein Zeichen nach links zu bewegen und somit das zuvor geschriebene Zeichen zu löschen.
// ("\b \b") wird der Effekt erzielt, dass das vorherige Zeichen durch ein Leerzeichen ersetzt und der Cursor wieder auf die Position des vorherigen Zeichens zurückgesetzt wird.
// index+5 wegen "You: " dies sind 5 Plätze dann wird das Löschen durch \b durchgeführt.
void f_delete(int index){
    for(int a=1; a <= index+5; a++){
        printf("\b \b");
    }
}

void f_chat(SOCKET clientSocket, char myid[]){
    char message[MAX_MESSAGE_LENGTH];
    char buf[MAX_BUFFER_LENGTH];
    
    fd_set read_fds;
    // read_fds: Ein Objekt vom Typ fd_set, das als Menge von Dateideskriptoren (File Descriptors) für die Verwendung in der select-Funktion dient. 
    // Hier wird es für die Überwachung des Client-Sockets verwendet.
    struct timeval timeout;
    // Ein Objekt vom Typ struct timeval, das für die Angabe eines Zeitlimits für die select-Funktion verwendet wird.
    // Hier wird es so konfiguriert, dass die select-Funktion sofort zurückkehrt, ohne auf Daten zu warten.

    while (1) {

        //! Für Zuhören bzw. warten auf Rückmeldung von clientServer
        FD_ZERO(&read_fds);
        // Die Funktion FD_ZERO wird verwendet, um die Menge der Dateideskriptoren zu leeren.
        // &read_fds ist der Zeiger auf die Menge (fd_set), die geleert wird.
        FD_SET(clientSocket, &read_fds);
        // Die Funktion FD_SET fügt einen bestimmten Dateideskriptor (clientSocket) zur Menge hinzu.
        // clientSocket ist der Dateideskriptor, der in die Menge aufgenommen wird.
        // &read_fds ist der Zeiger auf die Menge, zu der der Dateideskriptor hinzugefügt wird.
        //* ========
        //* Nach dem Aufruf dieser Funktion enthält die Menge den clientSocket als überwachten Dateideskriptor.
        //* ========
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        // Die struct timeval timeout wird auf Null gesetzt. Das bedeutet, dass die select-Funktion sofort zurückkehren wird,
        // ohne auf Daten zu warten. Es wird keine Verzögerung eingeführt.

        int ready = select(0, &read_fds, NULL, NULL, &timeout);
        // TODO: select() verstehen
        if (ready == SOCKET_ERROR) {
            perror("select");
            break;
        }

        if (ready > 0 && FD_ISSET(clientSocket, &read_fds)) {
            // Bedingung prüft, ob die select-Funktion Daten auf dem clientSocket gemeldet hat und ob der clientSocket in der Menge read_fds steht.
            packet myPacket;
            // in der Datei "packet.h" implementiert.
            int bytesReceived = recv(clientSocket, (char*)&myPacket, sizeof(myPacket), 0);
            // recv(1,2,3,4) 1: Socket aus dem Daten empfangen werden soll.
            // 2 buf: Ein Zeiger auf den Puffer, in den die empfangenen Daten geschrieben werden.
            // 3 len: Die maximale Länge des Puffers bzw. die maximale Anzahl an Bytes, die empfangen werden sollen.
            // 4 flags: Flags, die das Verhalten der Funktion steuern können (normalerweise 0).
            if (bytesReceived > 0) {
                printf("%s: %s\n", myPacket.sNum, myPacket.text);
            } else if (bytesReceived == 0) {
                printf("Connection closed by the client\n");
                break;
            } else {
                printf("Connection interrupted by the client\n");
                break;
            }
        }

        //! Überprüft, ob eine Taste auf der Tastatur gedrückt wurde.
        // wenn eine Taste gerückt ist, wird "You: " ausgegeben und index auf 0 initialisieren.
        if (_kbhit()) {
            printf("You: ");
            int index = 0;
            char ch;

            //! ab hier ist wiederholt aus der äußerten While-Schleife
            // wegen _kbhit() ist den Abschnitt "FD_ZERO()" bis int "ready" oben blockiert
            // deswegen wird while-Schleife erneuet angewendet => wareten wieder auf eine Rückmeldung von "Client"
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
                        printf("Connection closed by the client\n");
                        break;
                    } else {
                        printf("Connection interrupted by the client\n");
                        break;
                    }
                }

                if (_kbhit()) {
                    ch = _getch();
                    if (ch == 8 && index > 0) { // Überprüft, ob die gedrückte Taste die Backspace-Taste (ASCII-Wert 8) ist und ob index größer als 0 ist.
                        printf("\b \b"); // Löscht das vorherige Zeichen auf dem Bildschirm,
                                         // indem es den Cursor eine Position zurücksetzt, ein Leerzeichen ausgibt und den Cursor erneut zurücksetzt.
                        index--;
                    } else if (ch == 8 && index == 0) {
                        index = 0;
                    } else {
                        message[index++] = ch;
                        printf("%c", ch);
                    }

                    if (ch == '\r' || ch == '\n' || index >= sizeof(message) - 1) {
                        //Überprüft, ob die Eingabe beendet wurde, indem der Benutzer die Enter-Taste gedrückt hat ('\r' oder '\n') oder der message-Puffer voll ist.
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
            // Stellt sicher, dass das Feld sNum mit einem Nullzeichen abgeschlossen ist.
            // Dies ist wichtig, da die strcpy-Funktion das Nullzeichen am Ende einer Zeichenkette nicht kopiert.

            strcpy(myPacket.text, message);
            myPacket.text[sizeof(myPacket.text) - 1] = '\0';

            send(clientSocket, (void*)&myPacket, sizeof(myPacket), 0);

            printf("\n");
        }
    }
}

void f_close(SOCKET clientSocket, SOCKET serverSocket){
    printf("Closing Connection...\n");

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

int main(int argc,char* argv[]) {
    if(argc < 3){
        printf("Usage: %s <S-Nummer> <Port>",argv[0]);
        exit(1);
    }

    char myid[10];
    const char* PORT = argv[2];
    strncpy(myid, argv[1], sizeof(myid) - 1);

    SOCKET serverSocket;
    SOCKET clientSocket;
    struct addrinfo* result = NULL;
    struct addrinfo hints;

    f_wsadata();

    serverSocket = f_serverSocket();

    memset(&hints, 0, sizeof(hints));
    // Hier wird die Struktur hints für getaddrinfo() mit Nullen initialisiert,
    // um sicherzustellen,
    // dass alle ihre Felder vor dem Setzen von spezifischen Werten auf Null stehen.
    hints.ai_family = AF_INET6; // für IPv6
    hints.ai_socktype = SOCK_STREAM; // für TCP-Socket
    hints.ai_protocol = IPPROTO_TCP; // Port für TCP-Protokoll

    result = f_getaddrinfo(PORT, serverSocket, result, hints);

    f_bind(PORT,serverSocket, result);

    freeaddrinfo(result);

    f_listen(serverSocket);

    clientSocket = f_acc(serverSocket);

    f_chat(clientSocket, myid);

    f_close(clientSocket, serverSocket);

    return 0;
}
