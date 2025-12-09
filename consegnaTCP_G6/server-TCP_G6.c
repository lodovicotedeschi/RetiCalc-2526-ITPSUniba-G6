#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    // Avvio WinSock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Errore WSAStartup.\n");
        return 1;
    }

    // Creazione socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Errore creazione socket.\n");
        WSACleanup();
        return 1;
    }

    // Configurazione indirizzo
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5000);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Errore bind.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 1) == SOCKET_ERROR) {
        printf("Errore listen.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("[SERVER] In ascolto sulla porta 5000...\n");

    while (1) {
        // Accettazione connessione
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Errore accept.\n");
            continue;
        }

        printf("\n[SERVER] Client connesso.\n");

        // Invio messaggio iniziale
        const char *messaggio = "connessione avvenuta";
        send(client_socket, messaggio, strlen(messaggio) + 1, 0);

        while (1) {
            char lettera;
            int ricevitore = recv(client_socket, &lettera, 1, 0);

            if (ricevitore <= 0) {
                printf("[SERVER] Il client si e' disconnesso.\n");
                break;
            }

            char risposta[50];

            if (lettera == 'A' || lettera == 'a') strcpy(risposta, "ADDIZIONE");
            else if (lettera == 'S' || lettera == 's') strcpy(risposta, "SOTTRAZIONE");
            else if (lettera == 'M' || lettera == 'm') strcpy(risposta, "MOLTIPLICAZIONE");
            else if (lettera == 'D' || lettera == 'd') strcpy(risposta, "DIVISIONE");
            else strcpy(risposta, "TERMINE PROCESSO CLIENT");

            send(client_socket, risposta, strlen(risposta) + 1, 0);

            if (!strcmp(risposta, "TERMINE PROCESSO CLIENT")) {
                printf("[SERVER] Terminazione richiesta dal client.\n");
                break;
            }
            int nums[2];
            ricevitore = recv(client_socket, (char*)nums, sizeof(nums), 0);

            if (ricevitore <= 0) {
                printf("[SERVER] Il client ha chiuso prima di inviare i numeri.\n");
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

            send(client_socket, (char*)&risultato, sizeof(risultato), 0);

            printf("[SERVER] Operazione %s eseguita: %d e %d -> Risultato: %d\n", risposta, a, b, risultato);
        }

        // Chiusura connessione col client, attesa nuovo client
        closesocket(client_socket);
        printf("[SERVER] Connessione col client chiusa. In attesa di un nuovo client...\n");
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}