#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define LISTEN_QUEUE_SIZE 10
#define PING_INTERVAL 10
#define NAME_BUFFER_SIZE 50

// informacje o kliencie, jego socketFD , nazwie , i czy jest aktywny
typedef struct {
    int socket;
    char name[NAME_BUFFER_SIZE];
    int active;
    time_t last_response_time;
} client_t;

client_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; //aby operacje na clients byly atomowe

void broadcast_message(const char *message_buffer, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].socket != sender_socket) {
            if (send(clients[i].socket, message_buffer, strlen(message_buffer), 0) < 0) {
                perror("Sending message to client failed");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int is_client_active(int idx){ //tylko dla petli whlie
    pthread_mutex_lock(&clients_mutex);
    int res = clients[idx].active;
    pthread_mutex_unlock(&clients_mutex);
    return res;
}

void *handle_client_main(void *arg) {
    int client_socket = *((int *)arg); //file dc
    char buffer[BUFFER_SIZE];
    char name_buffer[NAME_BUFFER_SIZE];
    int client_index = -1; //numer klienta

    //pierwsza wiadomosc to name_buffer od clienta
    recv(client_socket, name_buffer, sizeof(name_buffer), 0);

    pthread_mutex_lock(&clients_mutex);
    // szukanie wolnego miejsca aby zarejestrowac clienta
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].socket = client_socket;
            strcpy(clients[i].name, name_buffer);
            clients[i].active = 1;
            clients[i].last_response_time = time(NULL); //now time
            client_index = i;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (client_index == -1) {
        printf("Server is full\n");
        close(client_socket);
        return NULL;
    }

    printf("Klient %s sie podlaczyl\n", clients[client_index].name);

    while (1) {
        if (!is_client_active(client_index)){
            break; // zakoncza watek, klient nie jest aktywny
        }

        int received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (received <= 0) {
            printf("ERR Klient %s najprawdopodobniej nie funkcjonuje, wyrejestrowywanie...\n", clients[client_index].name);
            close(client_socket);
            pthread_mutex_lock(&clients_mutex);
            clients[client_index].active = 0;
            pthread_mutex_unlock(&clients_mutex);
            break;
        }

        buffer[received] = '\0';

        // log response time (wszytskie req)
        pthread_mutex_lock(&clients_mutex);
        clients[client_index].last_response_time = time(NULL);
        pthread_mutex_unlock(&clients_mutex);
        

        // LIST: Pobranie z serwera i wylistowanie wszystkich aktywnych klientów
        if (strncmp(buffer, "LIST", sizeof("LIST")-1) == 0) {
            pthread_mutex_lock(&clients_mutex);
            char responce_buffer[BUFFER_SIZE] = "Aktywni clienci:\n";
            for (int i = 0; i < MAX_CLIENTS; i++) { //doklejanie do stringa nazw clientow
                if (clients[i].active) {
                    strcat(responce_buffer, clients[i].name);
                    strcat(responce_buffer, "\n");
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            send(client_socket, responce_buffer, strlen(responce_buffer), 0); //wyslij do klienta tego stringa
        } 
        
        // 2ALL: Wysłanie wiadomości do wszystkich aktywnych klientów,
        // client wysyla stringa, serwer rozsyla go do wszystkich aktywnych klientow
        // // oraz dopisuje do tego date
        else if (strncmp(buffer, "2ALL", sizeof("2ALL")-1) == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);

            char responce_buffer[BUFFER_SIZE];
            snprintf(responce_buffer, sizeof(responce_buffer), "%02d:%02d:%02d %s: %s", t->tm_hour, t->tm_min, t->tm_sec, clients[client_index].name, buffer + 5);
            broadcast_message(responce_buffer, client_socket); //rozeslij po wszytskich
        } 
        
        
        // 2ONE Wysłanie wiadomości do konkretnego klienta. 
        // Klient wysyła do serwera string podając jako adresata 
        // konkretnego klienta o identyfikatorze z listy aktywnych klientów. 
        // Serwer wysyła ten ciąg wraz z identyfikatorem klienta-nadawcy 
        // oraz aktualną datą do wskazanego klienta.
        else if (strncmp(buffer, "2ONE", sizeof("2ONE")-1) == 0) {
            char *name = strtok(buffer + 5, " "); //nazwa od 2ONE_ do _ (spacji)
            char *msg = strtok(NULL, ""); //sprytny sposob na wyciagniecie wiadomosci tokenizacja -> daje 2 token (strtok ma wewnetrzy wskaznik)

            time_t now = time(NULL);
            struct tm *t = localtime(&now); //na strukture danych (godzina)

            char message[BUFFER_SIZE];
            snprintf(message, sizeof(message), "%02d:%02d:%02d %s: %s", t->tm_hour, t->tm_min, t->tm_sec, clients[client_index].name, msg);

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && strcmp(clients[i].name, name) == 0) { //mam wyszukiwanie po imieniu tego clienta
                    send(clients[i].socket, message, strlen(message), 0);
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } 
        
        
        
        else if (strncmp(buffer, "STOP", sizeof("STOP")-1) == 0) {
            printf("Klient %s zglosil zamkniecie siebie\n", clients[client_index].name);
            close(client_socket); //zamknij fd socketu do klienta
            pthread_mutex_lock(&clients_mutex);
            clients[client_index].active = 0;
            pthread_mutex_unlock(&clients_mutex);
            break; //wyjdz z petli i skoncz watek klienta
        }
    }

    return NULL;
}

void *ping_clients_main(void *arg) {
    while (1) {
        sleep(PING_INTERVAL);

        pthread_mutex_lock(&clients_mutex);
        time_t now = time(NULL);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                if (difftime(now, clients[i].last_response_time) > PING_INTERVAL) {
                    printf("Usuwanie nieaktywnego klienta: %s\n ...", clients[i].name);

                    clients[i].active = 0; // oznacz jako nieaktywny
                    close(clients[i].socket); // zamknij deksryptor dla tego klienta

                } else {
                    send(clients[i].socket, "ALIVE", strlen("ALIVE"), 0);
                    printf("wyslano ALIVE do %s\n" , clients[i].name);
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // Check the number of command line arguments
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <adres IPv4> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *address = argv[1];
    int port = atoi(argv[2]);

    // socket fd
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); //IPv4 , tcp_socket , 0 -> socket fd
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    struct sockaddr_in server_addr; //serwer address IPv4
    server_addr.sin_family = AF_INET; //IPv4
    server_addr.sin_addr.s_addr = inet_addr(address); //parsuje string na IP 
    server_addr.sin_port = htons(port); //numer portu

    // zbinduj socket do tego adresu
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        printf("HINT Moze port jest juz zajety?");
        exit(EXIT_FAILURE);
    }

    // sluchaj na tym sockecie, i kolejkuj do LISTEN_QUEUE_SIZE req
    if (listen(server_socket, LISTEN_QUEUE_SIZE) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server slucha na  %s:%d\n", address, port);

    pthread_t ping_thread;
    pthread_create(&ping_thread , NULL , ping_clients_main , NULL);


    while (1) {
        // akceptuj nowe polaczenie od clienta
        struct sockaddr_in client_addr; //client address IPv4
        socklen_t client_addr_len = sizeof(client_addr); //rozmiar adresu klienta
        
        // file dc klienta (z socketa)
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // nowy watek co bedzie go obslugiwal
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client_main, &client_socket);
        pthread_detach(client_thread);
    }

    // Close the server socket
    close(server_socket);
    return 0;
}
