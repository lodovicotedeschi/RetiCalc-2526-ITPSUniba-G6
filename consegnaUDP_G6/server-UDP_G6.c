#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[100];

    // Avvio WinSock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Errore WSAStartup.\n");
        return 1;
    }

    // Creazione socket UDP
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Errore creazione socket.\n");
        WSACleanup();
        return 1;
    }

    // Configurazione indirizzo
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5000);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Errore bind.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("[SERVER] UDP in ascolto sulla porta 5000...\n");

    while (1) {
        printf("\n[SERVER] In attesa del primo messaggio dal client...\n");

        // PRIMO MESSAGGIO
        memset(buffer, 0, sizeof(buffer));
        recvfrom(server_socket, buffer, sizeof(buffer), 0,
                 (struct sockaddr*)&client_addr, &client_len);

        printf("[SERVER] Client rilevato: %s\n", buffer);

        // Invio messaggio iniziale
        const char *messaggio = "connessione avvenuta";
        sendto(server_socket, messaggio, strlen(messaggio) + 1, 0,
               (struct sockaddr*)&client_addr, client_len);

        // CICLO DI COMUNICAZIONE
        while (1) {
            char lettera;

            int ricevitore = recvfrom(server_socket, &lettera, 1, 0,
                               (struct sockaddr*)&client_addr, &client_len);

            if (ricevitore <= 0) {
                printf("[SERVER] Errore o client non raggiungibile.\n");
                break;
            }

            char risposta[50];

            if (lettera == 'A' || lettera == 'a') strcpy(risposta, "ADDIZIONE");
            else if (lettera == 'S' || lettera == 's') strcpy(risposta, "SOTTRAZIONE");
            else if (lettera == 'M' || lettera == 'm') strcpy(risposta, "MOLTIPLICAZIONE");
            else if (lettera == 'D' || lettera == 'd') strcpy(risposta, "DIVISIONE");
            else strcpy(risposta, "TERMINE PROCESSO CLIENT");

            sendto(server_socket, risposta, strlen(risposta) + 1, 0,
                   (struct sockaddr*)&client_addr, client_len);

            if (!strcmp(risposta, "TERMINE PROCESSO CLIENT")) {
                printf("[SERVER] Terminazione richiesta dal client.\n");
                break;
            }

            // Ricezione numeri
            int nums[2];
            ricevitore = recvfrom(server_socket, (char*)nums, sizeof(nums), 0,
                           (struct sockaddr*)&client_addr, &client_len);

            printf("[SERVER] Numeri ricevuti: %d, %d per operazione %s\n", nums[0], nums[1], risposta);

            if (ricevitore <= 0) {
                printf("[SERVER] Client disconnesso prima di inviare i numeri.\n");
                break;
            }

            int a = nums[0];
            int b = nums[1];
            int risultato = 0;

            if (!strcmp(risposta, "ADDIZIONE")) risultato = a + b;
            else if (!strcmp(risposta, "SOTTRAZIONE")) risultato = a - b;
            else if (!strcmp(risposta, "MOLTIPLICAZIONE")) risultato = a * b;
            else if (!strcmp(risposta, "DIVISIONE")) {
                if (b == 0) risultato = 0;
                else risultato = a / b;
            }

            // Invio risultato
            sendto(server_socket, (char*)&risultato, sizeof(risultato), 0,
                   (struct sockaddr*)&client_addr, client_len);
            
            // Invio del messaggio finale al client
            const char *fine = "FINE";
            sendto(server_socket, fine, strlen(fine) + 1, 0,
            (struct sockaddr*)&client_addr, client_len);

            printf("[SERVER] Operazione completata, messaggio FINE inviato.\n");

            break; // termina il ciclo della sessione client

        }

        printf("[SERVER] Sessione client terminata. Attesa nuovo client...\n");

    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}