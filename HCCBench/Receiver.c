/*
█████   █████                   ███████████                               █████      ███████████  
░░███   ░░███                   ░░███░░░░░███                             ░░███      ░░███░░░░░███ 
 ░███    ░███   ██████   ██████  ░███    ░███  ██████  ████████    ██████  ░███████   ░███    ░███ 
 ░███████████  ███░░███ ███░░███ ░██████████  ███░░███░░███░░███  ███░░███ ░███░░███  ░██████████  
 ░███░░░░░███ ░███ ░░░ ░███ ░░░  ░███░░░░░███░███████  ░███ ░███ ░███ ░░░  ░███ ░███  ░███░░░░░███ 
 ░███    ░███ ░███  ███░███  ███ ░███    ░███░███░░░   ░███ ░███ ░███  ███ ░███ ░███  ░███    ░███ 
 █████   █████░░██████ ░░██████  ███████████ ░░██████  ████ █████░░██████  ████ █████ █████   █████
░░░░░   ░░░░░  ░░░░░░   ░░░░░░  ░░░░░░░░░░░   ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░ ░░░░░ ░░░░░   ░░░░░ 

Yet another proggi to test your network applications on the fly - By Sash Electrique [Sacha Tholl].

Open Receiverconfig.txt in your notepad or so:
Edit the port you expect the packets to come from
Edit the maximum size of the packets you want to receive
Run Receiver.exe in a CMD (always before Sender.exe)
Start Sender.exe
Wait until Sender.exe is finished --> Usually Receiver will shut down - if not, kill it with ctrl-c or so
Make some statistics in Excel, R or whatever with the data in Senderoutput and Receiveroutput.

Tip: Save your output files, otherwise the data will be overwritten!!!!
*/

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CONFIG_FILE "receiverconfig.txt"
#define BUFFER_SIZE 65536

void read_config(int *server_port, int *buffer_size) {
    FILE *config_file = fopen(CONFIG_FILE, "r");
    if (config_file != NULL) {
        char buffer[BUFFER_SIZE];
        if (fgets(buffer, BUFFER_SIZE, config_file) != NULL) {
            buffer[strcspn(buffer, "\r\n")] = '\0'; // Entfernen von Zeilenumbruechen
            *server_port = atoi(buffer);
        }
        if (fgets(buffer, BUFFER_SIZE, config_file) != NULL) {
            buffer[strcspn(buffer, "\r\n")] = '\0'; // Entfernen von Zeilenumbruechen
            *buffer_size = atoi(buffer);
        }
        fclose(config_file);
    } else {
        perror("Fehler beim Oeffnen der Konfigurationsdatei");
        exit(1);
    }
}

void write_output(const char *content) {
    FILE *output_file = fopen("receiveroutput.txt", "a");
    if (output_file != NULL) {
        fprintf(output_file, "%s\n", content);
        fclose(output_file);
    } else {
        perror("Fehler beim Schreiben der Ausgabedatei");
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Fehler bei der Initialisierung von Winsock");
        return 1;
    }
#endif

    int server_port, buffer_size;

    read_config(&server_port, &buffer_size);

    printf("Empfaenger gestartet. Warte auf Daten...\n");
    printf("Server Port: %d, Buffer Groessee: %d\n", server_port, buffer_size);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Fehler beim Erstellen des Sockets");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Fehler beim Binden des Sockets");
        return 1;
    }

    if (listen(sockfd, 1) < 0) {
        perror("Fehler beim Hoeren auf eingehende Verbindungen");
        return 1;
    }

    char buffer[buffer_size];
#ifdef _WIN32
    int client_addr_len = sizeof(client_addr);
#else
    socklen_t client_addr_len = sizeof(client_addr);
#endif

    int client_socket = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        perror("Fehler beim Akzeptieren der Verbindung");
        return 1;
    }

     int packet_count = 0; // Hier wird packet_count deklariert und initialisiert
     
    while (1) {
        int bytes_received = recv(client_socket, buffer, buffer_size - 1, 0);

        if (bytes_received < 0) {
            perror("Fehler beim Empfangen von Daten");
            break;
        }

        if (bytes_received == 0) {
            printf("Verbindung zum Client geschlossen.\n");
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminieren des empfangenen Datenstroms

        // Aktuellen Zeitstempel erhalten mit Nano-Sekunden-Aufloesung
        struct timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        char timestamp[100];
        snprintf(timestamp, sizeof(timestamp), "%ld.%09ld", current_time.tv_sec, current_time.tv_nsec);

        // printf("Paket %d: Empfangen um %s: %d Bytes - Inhalt: %s\n", ++packet_count, timestamp, bytes_received, buffer);
        printf("Paket: %d -  Bytes: %d - Empfangen: %s - Inhalt: %s\n",  ++packet_count, bytes_received, timestamp, buffer);

        // Ausgabe in die Datei schreiben
        char output_content[BUFFER_SIZE];
        // snprintf(output_content, sizeof(output_content), "Empfangen um %s - Paket %d: %d Bytes - Inhalt: %s", timestamp, packet_count, bytes_received, buffer);
        snprintf(output_content, sizeof(output_content), "%d - %d - %s - %s", packet_count, bytes_received, timestamp, buffer);
        write_output(output_content);
    }

#ifdef _WIN32
    closesocket(client_socket);
    closesocket(sockfd);
    WSACleanup();
#else
    close(client_socket);
    close(sockfd);
#endif

    return 0;
}
