#ifndef SPEC_H
#define SPEC_H

#define MESSAGE_BUFFER_SIZE 1024
#define SERVER_QUEUE_NAME "/server_queue"
#define CLIENT_QUEUE_NAME "/client_queue"

#define CLIENT_QUEUE_NAME_SIZE 50
#define MAX_CLIENTS_NO 10

// enum dla typu wiadomosci
typedef enum {
    INIT, 
    IDENTIFIER,
    MESSAGE_TEXT,
    CLIENT_CLOSE
} message_type_t;

// sama wiadomosc
typedef struct {
    message_type_t type;

    int identifier;
    char text[MESSAGE_BUFFER_SIZE];
} message_t;

#endif