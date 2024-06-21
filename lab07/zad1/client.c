#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "spec.h"

volatile bool is_running = true; //moze sie zmienic w dowolnym momencie

void SIGNAL_handler(int signum) { //obsluga sygnalu -> wyjscie z petli -> pozamykanie wszystkiego
    is_running = false;
}

int main() {
    //identyfikator kolejki z uzyciem pid tego procesu jako np. /client_queue_1234
    pid_t pid = getpid();
    char queue_name[CLIENT_QUEUE_NAME_SIZE];
    snprintf(queue_name, sizeof(queue_name), "%s_%d", CLIENT_QUEUE_NAME, pid); 

    // obcje dla kolejki klienta
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_msgsize = sizeof(message_t),
        .mq_maxmsg = 10
    };
    
    //otwarcie kolejki klienta
    mqd_t mq_client_descriptor = mq_open(queue_name, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attributes);
    if(mq_client_descriptor < 0){perror("mq_open client");}
        
    //otwarcie kolejki serwera
    mqd_t mq_server_descriptor = mq_open(SERVER_QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if(mq_server_descriptor < 0) {
        printf("ERR Mozliwe ze serwer nie uruchomiony!\n");
        perror("mq_open server");
        exit(EXIT_FAILURE);
    }

    //INIT message dla serwera
    message_t message_init = {
        .type = INIT,
        .identifier = -1,
    };

    memcpy(message_init.text, queue_name, strlen(queue_name)); //message_init.text = queue_name

    // send INIT message to server
    if(mq_send(mq_server_descriptor, (char*)&message_init, sizeof(message_init), 1) < 0){
        perror("mq_send init");
        exit(EXIT_FAILURE);
    }

    // parent (writing to server) -> child (reading from server)
    int to_parent_pipe[2];
    if(pipe(to_parent_pipe) < 0){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //zarejestrowanie obslugi SIGINT
    signal(SIGINT, SIGNAL_handler);

    // dziecko nasluchujace na wiadomosci od serwera
    pid_t fork_id = fork();
    if (fork_id < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
        
    if (fork_id == 0) { //dziecko
        close(to_parent_pipe[0]); //nie czytaj, bedzie pisane
        message_t receive_message;

        while(is_running) {
            //odbieraj wiadomosci,
            mq_receive(mq_client_descriptor, (char*)&receive_message, sizeof(receive_message), NULL);

            switch(receive_message.type) {
                case MESSAGE_TEXT: //tekst wiec go wypisz
                    printf("Odebrano z clienta o id: %d wiadomosc\n", receive_message.identifier);
                    printf("> Tresc: %s\n", receive_message.text);
                    break;

                case IDENTIFIER: //odebranie identyfikatora swojego
                    printf("Odebrano swoj identyfikator z serwera: %d\n", receive_message.identifier);
                    // przeslij identyfikator do rodzica
                    write(to_parent_pipe[1], &receive_message.identifier, sizeof(receive_message.identifier));
                    break;

                default:
                    printf("ERR DEFAULT Nieznany typ wiadomosci\n");
                    break;
            }
        }
        printf("Konczenie procesu dziecka...\n");
        exit(0);
    } else { //rodzic
        close(to_parent_pipe[1]); //nie pisz, bedzie czytane
        int id = -1;

        if(read(to_parent_pipe[0], &id, sizeof(id)) < 0){ //zczytaj id z potomka
            perror("read identifier");
            exit(EXIT_FAILURE);
        } 
            

        char buffer[MESSAGE_BUFFER_SIZE];
        while(is_running) {
            printf("Wpisz wiadomosc: ");
            if (fgets(buffer, MESSAGE_BUFFER_SIZE, stdin)) {
                // Usunięcie znaku nowej linii, jeśli istnieje
                buffer[strcspn(buffer, "\n")] = '\0';
            } else {
                printf("ERR Blad podczas wczytywania z stdin.\n");
                continue;
            }

            //wiadomosci do wyslania
            message_t send_message = {
                .type = MESSAGE_TEXT,
                .identifier = id
            };

            memcpy(send_message.text, buffer, strlen(buffer)); // send_message.text = buffer
            mq_send(mq_server_descriptor, (char*)&send_message, sizeof(send_message), 1);

        }
        //closing 
        printf("Zamykanie procesu rodzica klienta...\n");

        if(id != -1){ //informowanie serwera o zamknieciu klienta kiedy jest zarejstrowany
            message_t message_close = {
                .type = CLIENT_CLOSE,
                .identifier = id
            };
            /* Notify server that client is closing */
            mq_send(mq_server_descriptor, (char*)&message_close, sizeof(message_close), 1);
        }
        // zamkniecie kolejek
        mq_close(mq_server_descriptor);
        mq_close(mq_client_descriptor);

        // usuniecie kolejki klienta
        mq_unlink(queue_name);
    }
}