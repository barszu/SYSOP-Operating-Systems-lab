#ifndef SPEC_H
#define SPEC_H

#define SHARED_MEMORY_DESCRIPTOR_NAME "shared_memory"
#include <semaphore.h>

#define MAX_PRINTERS 100
#define MAX_PRINTER_BUFFER_SIZE 256

//semafor (stan->ile czeka do druku (na - | 0), 1 to wolna) | buffer na stringa
typedef struct {
    sem_t printer_semaphore;
    char printer_buffer[MAX_PRINTER_BUFFER_SIZE];
} printer_t;

typedef struct {
    printer_t printers[MAX_PRINTERS];
    int printers_no; //rzeczywista liczba drukarek
} memory_map_t;

#endif //SPEC_H