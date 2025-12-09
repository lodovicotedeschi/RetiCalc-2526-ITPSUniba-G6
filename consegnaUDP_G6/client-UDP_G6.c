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
    int server_addr_len = sizeof(server_addr);

    // Avvio WinSock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Errore WSAStartup\n");
        return 1;
    }

    // Utente inserisce nome server
    printf("Inserisci il nome del server (es. localhost): ");
    scanf("%s", server_name);

    // Risoluzione DNS
    struct hostent *host = gethostbyname(server_name);
    if (host == NULL) {
        printf("Errore risoluzione del nome\n");
        WSACleanup();
        return 1;
    }

    struct in_addr addr;
    memcpy(&addr, host->h_addr, host->h_length);
    const char *server_ip = inet_ntoa(addr);

    printf("IP risolto: %s\n", server_ip);

    // Creazione socket UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Errore creazione socket\n");
        WSACleanup();
        return 1;
    }

    // Preparo indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(5000);

    // Primo scambio
    char buffer[100];
    char conferma[] = "HELLO";
    sendto(sock, conferma, strlen(conferma)+1, 0,
           (struct sockaddr*)&server_addr, server_addr_len);

    // Attesa risposta "connessione avvenuta"
    recvfrom(sock, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&server_addr, &server_addr_len);

    printf("SERVER: %s\n", buffer);

    // Invio lettera operazione
    char lettera;
    printf("Inserisci lettera (A/S/M/D): ");
    scanf(" %c", &lettera);

    sendto(sock, &lettera, 1, 0,
           (struct sockaddr*)&server_addr, server_addr_len);

    // Ricezione operazione
    memset(buffer, 0, sizeof(buffer));
    recvfrom(sock, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&server_addr, &server_addr_len);

    printf("SERVER: %s\n", buffer);

    if (!strcmp(buffer, "TERMINE PROCESSO CLIENT")) {
        closesocket(sock);
        WSACleanup();
        return 0;
    }

    // Invio due numeri
    int a, b;
    printf("Inserisci primo intero: ");
    scanf("%d", &a);
    printf("Inserisci secondo intero: ");
    scanf("%d", &b);

    int nums[2] = {a, b};

    sendto(sock, (char*)nums, sizeof(nums), 0,
           (struct sockaddr*)&server_addr, server_addr_len);

    // Ricezione risultato
    int risultato = 0;
    recvfrom(sock, (char*)&risultato, sizeof(risultato), 0,
         (struct sockaddr*)&server_addr, &server_addr_len);

    printf("Risultato: %d\n", risultato);

    // Ricezione messaggio finale "FINE"
    memset(buffer, 0, sizeof(buffer));
    recvfrom(sock, buffer, sizeof(buffer), 0,
         (struct sockaddr*)&server_addr, &server_addr_len);

    printf("SERVER: %s\n", buffer);
    closesocket(sock);
    WSACleanup();
    return 0;
}