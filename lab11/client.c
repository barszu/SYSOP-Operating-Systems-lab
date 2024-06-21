#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#include <netinet/in.h> //struct sockaddr_in IPv4

#define BUFFER_SIZE 1024
#define NAME_BUFFER_SIZE 50

int client_socket;
char client_name[NAME_BUFFER_SIZE];

void handle_exit() {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "STOP"); //wpisz do buffera "STOP"
    send(client_socket, buffer, strlen(buffer), 0); //wyslij
    close(client_socket); //zamknij polaczenie
    exit(0);
}

void *thread_main(void *arg) {
    // odpowiedzialny tylko za odbieranie, i odpowiadanie na pingi odrazu
    char buffer[BUFFER_SIZE];
    while (1) {
        //ile byteow odebrano
        int received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (received <= 0) {
            printf("Disconnected from server\n");
            exit(1);
        }
        buffer[received] = '\0';

        if (strncmp(buffer, "ALIVE", sizeof("ALIVE")-1) == 0) {
            send(client_socket, "ALIVE" , strlen("ALIVE"), 0);
        } else {
            printf("Otrzymano: %s\n", buffer);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <client_name> <address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(client_name, argv[1]);
    const char *address = argv[2];
    int port = atoi(argv[3]);

    client_socket = socket(AF_INET, SOCK_STREAM, 0); //IPv4 , tcp_socket , 0 -> socket fd
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr; //serwer address IPv4
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(address); //parsuje string na IP 
    server_addr.sin_port = htons(port); //numer portu

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { //podlacz sie
        perror("Connect failed");
        close(client_socket);
        exit(EXIT_FAILURE); 
    }

    //wysyla na serwer client_name jako string, bez flag
    send(client_socket, client_name, strlen(client_name), 0);

    signal(SIGINT, handle_exit);

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, thread_main, NULL);

    // watek glowny odpowiedzialny za wysylanie
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    return 0;
}
