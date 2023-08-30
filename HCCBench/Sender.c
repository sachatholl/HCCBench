/*
 █████   █████                   ███████████                               █████       █████████ 
░░███   ░░███                   ░░███░░░░░███                             ░░███       ███░░░░░███
 ░███    ░███   ██████   ██████  ░███    ░███  ██████  ████████    ██████  ░███████  ░███    ░░░ 
 ░███████████  ███░░███ ███░░███ ░██████████  ███░░███░░███░░███  ███░░███ ░███░░███ ░░█████████ 
 ░███░░░░░███ ░███ ░░░ ░███ ░░░  ░███░░░░░███░███████  ░███ ░███ ░███ ░░░  ░███ ░███  ░░░░░░░░███
 ░███    ░███ ░███  ███░███  ███ ░███    ░███░███░░░   ░███ ░███ ░███  ███ ░███ ░███  ███    ░███
 █████   █████░░██████ ░░██████  ███████████ ░░██████  ████ █████░░██████  ████ █████░░█████████ 
░░░░░   ░░░░░  ░░░░░░   ░░░░░░  ░░░░░░░░░░░   ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░ ░░░░░  ░░░░░░░░░  

Yet another proggi to test your network applications on the fly - By Sash Electrique [Sacha Tholl].

Open Senderconfig.txt in your notepad or so:
Edit the IP to which you want to send the test packets
Edit the port on which you want to smash your packets
Start Sender.exe in a CMD
Enter the desired packet width in bytes (but max. one byte less than configured in receiverconfig.txt...yeah its a bug, I know!!!)
Enter the number of packets you want to send
Enter the milliseconds of sleep time between consecutive packets
Flood your network...
Make some statistics in Excel, R, or whatever using the data in Senderoutput and Receiveroutput.

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

#define CONFIG_FILE "senderconfig.txt"
#define OUTPUT_FILE "senderoutput.txt"


void generate_random_data(char *data, int size) {
    int i;
    for (i = 0; i < size - 1; ++i) {
        data[i] = 'A' + rand() % 26;
    }
    data[size - 1] = '\0';
}

void read_config(char *ip, int *port, int *packet_size, int *num_packets, int *delay_ms) {
    FILE *config_file = fopen(CONFIG_FILE, "r");
    if (config_file == NULL) {
        perror("Fehler beim Oeffnen der Konfigurationsdatei");
        exit(1);
    }

    fscanf(config_file, "%s", ip);
    fscanf(config_file, "%d", port);

    fclose(config_file);

    printf("Server IP: %s\n", ip);
    printf("Port: %d\n", *port);

    printf("Geben Sie die Paketgroesse ein: ");
    scanf("%d", packet_size);

    printf("Geben Sie die Anzahl der zu sendenden Pakete ein: ");
    scanf("%d", num_packets);

    printf("Geben Sie die Verzoegerungszeit zwischen den Paketen in Millisekunden ein: ");
    scanf("%d", delay_ms);
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Fehler beim Initialisieren von Winsock.\n");
        return 1;
    }
#endif

    char SERVER_IP[100];
    int SERVER_PORT, packet_size, num_packets, delay_ms;
    read_config(SERVER_IP, &SERVER_PORT, &packet_size, &num_packets, &delay_ms);

    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Fehler beim Erstellen des Sockets");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    printf("Verbinde mit Server IP: %s, Port: %d\n", SERVER_IP, SERVER_PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Fehler beim Verbinden zum Server");
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
        return 1;
    }

    char *packet = (char *)malloc(packet_size);

    if (packet == NULL) {
        perror("Fehler beim Allozieren des Speichers fuer das Datenpaket");
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
        return 1;
    }

    FILE *output_file = fopen(OUTPUT_FILE, "w");
    if (output_file == NULL) {
        perror("Fehler beim Oeffnen der Ausgabedatei");
        free(packet);
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
        return 1;
    }

    printf("Verbindung zum Server hergestellt. Starte das Senden von %d Paketen mit der Groesse %d Bytes.\n", num_packets, packet_size);

    int packet_count = 0;
    int i;

    for (i = 0; i < num_packets; ++i) {
        generate_random_data(packet, packet_size);
        if (send(sockfd, packet, packet_size, 0) < 0) {
            perror("Fehler beim Senden");
            break;
        }

        // Zeitstempel des versendeten Pakets erhalten und formatieren
        struct timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        char timestamp[100];
        snprintf(timestamp, sizeof(timestamp), "%ld.%09ld", current_time.tv_sec, current_time.tv_nsec);

        // printf("Gesendetes Paket %d - Zeitstempel: %s, Inhalt: %s\n", ++packet_count, timestamp, packet);
        printf("Paket: %d - Bytes: %d - Zeitstempel: %s - Inhalt: %s\n", ++packet_count, packet_size, timestamp, packet);
        fprintf(output_file, "%d - %d - %s - %s\n", packet_count, packet_size, timestamp, packet);

        if (delay_ms > 0) {
#ifdef _WIN32
            Sleep(delay_ms);
#else
            struct timespec sleep_time;
            sleep_time.tv_sec = delay_ms / 1000;
            sleep_time.tv_nsec = (delay_ms % 1000) * 1000000;
            nanosleep(&sleep_time, NULL);
#endif
        }
    }

    free(packet);
    fclose(output_file);

#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif

    return 0;
}
