#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char server_name[50];

    // Avvio WinSock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Errore WSAStartup\n");
        return 1;
    }

    // Utente inserisce nome server
    printf("Inserisci il nome del server (es. localhost): ");
    scanf("%s", server_name);

    // Risoluzione nome → IP
    struct hostent *host = gethostbyname(server_name);
    if (host == NULL) {
        printf("Errore risoluzione del nome (gethostbyname).\n");
        WSACleanup();
        return 1;
    }

    // Copia dell’indirizzo IP risolto
    struct in_addr addr;
    memcpy(&addr, host->h_addr, host->h_length);
    const char *server_ip = inet_ntoa(addr);

    printf("IP risolto: %s\n", server_ip);

    // Creazione socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Errore creazione socket\n");
        WSACleanup();
        return 1;
    }

    // Preparo struttura per la connessione
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(5000);

    // Connessione
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Impossibile connettersi al server.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Ricezione "connessione avvenuta"
    char buffer[100];
    recv(sock, buffer, sizeof(buffer), 0);
    printf("SERVER: %s\n", buffer);

    // Invio lettera
    char lettera;
    printf("Inserisci lettera (A/S/M/D): ");
    scanf(" %c", &lettera);

    send(sock, &lettera, 1, 0);

    // Ricezione operazione
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("SERVER: %s\n", buffer);

    if (!strcmp(buffer, "TERMINE PROCESSO CLIENT")) {
        closesocket(sock);
        WSACleanup();
        return 0;
    }

    int a, b;
    printf("Inserisci primo intero: ");
    scanf("%d", &a);
    printf("Inserisci secondo intero: ");
    scanf("%d", &b);

    int nums[2] = {a, b};
    send(sock, (char*)nums, sizeof(nums), 0);

    // Ricezione risultato
    int risultato;
    recv(sock, (char*)&risultato, sizeof(risultato), 0);

    printf("Risultato: %d\n", risultato);

    closesocket(sock);
    WSACleanup();
    return 0;
}