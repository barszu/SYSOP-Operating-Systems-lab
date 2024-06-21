#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>

#include "spec.h"

int main() {
    // obcje dla kolejki serwera
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_msgsize = sizeof(message_t),
        .mq_maxmsg = 10
    };

    mqd_t client_queues[MAX_CLIENTS_NO];
    for (int i = 0; i < MAX_CLIENTS_NO; i++){client_queues[i] = -1;} // -1 brak clienta 

    // kolejka serwera
    mqd_t mq_descriptor = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR , &attributes); 
    if(mq_descriptor < 0){perror("mq_open server");}
        
    message_t receive_message;

    while(1) {
        mq_receive(mq_descriptor, (char*)&receive_message, sizeof(receive_message), NULL); //odebranie z serwer queue

        switch(receive_message.type) {
            case INIT: //klient chce sie zarejsstrowac
                int id = 0;
                for (int i=0 ; i < MAX_CLIENTS_NO; i++) {
                    if(client_queues[i] == -1) {
                        id = i;
                        break;
                    }
                }
                if(id == MAX_CLIENTS_NO){
                    perror("INIT max number of clients\n");
                    continue; // skip INIT
                }
                

                //otwarcie i zapisanie kolejki klienta
                client_queues[id] = mq_open(receive_message.text, O_RDWR, S_IRUSR | S_IWUSR, NULL);
                if(client_queues[id] < 0){perror("INIT mq_open client");}
                    

                /* Send identifier to client */
                message_t send_message = {
                    .type = IDENTIFIER,
                    .identifier = id,
                };

                mq_send(client_queues[id], (char*)&send_message, sizeof(send_message), 1); //force zrzutowanie na char*, ale struktura sie zachowa
                printf("LOG: Nowy klient z id: %d\n zarejestrowany!\n", id);
                break;

            case MESSAGE_TEXT: //odebranie wiadomosci od klienta i rozeslanie do pozostalych
                for (int i = 0; i < MAX_CLIENTS_NO; i++){
                    if((i != receive_message.identifier) && (i != -1)){ //nie wysylamy do nadawcy i nieistniejacych
                        mq_send(client_queues[i], (char*)&receive_message, sizeof(receive_message), 1);
                    }
                }
                printf("LOG: Wiadomosc od klienta z id: %d rozeslana!\n", receive_message.identifier);
                printf("LOG: Wiadomosc: %s\n", receive_message.text);
                break;

            case CLIENT_CLOSE: //klient sie zamyka, wyrestruj go
                mq_close(client_queues[receive_message.identifier]); //wyrejestrowanie klienta
                client_queues[receive_message.identifier] = -1; //ustawienie na -1
                printf("LOG: Wyrejestrowanie klienta z id: %d ! \n", receive_message.identifier);
                break;

            default:
                printf("ERR DEFAULT Nieznany typ wiadomosci\n");
                break;
        }
    }

    printf("Wylaczanie serwera...\n");

    // zamkniecie kolejek klientow
    for (int i = 0; i < MAX_CLIENTS_NO; i++){
        if(client_queues[i] != -1){mq_close(client_queues[i]);}
    }

    // zamkniecie i usuniecie (odlinkowanie) kolejki serwera
    mq_close(mq_descriptor);
    mq_unlink(SERVER_QUEUE_NAME);

    return 0;
}