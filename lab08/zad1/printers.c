#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <semaphore.h>

#include "specs.h"

volatile bool is_running = true;
int PRINTERS_NO = 5;

void SIGNAL_handler(int signum) {
    is_running = false;
    printf("Zamykanie drukarek...\n");
}

int main() {

    // otworz deskryptor do pamieci wspoldzielonej, stworz ja
    int memory_fd = shm_open(SHARED_MEMORY_DESCRIPTOR_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR);
    if(memory_fd < 0){perror("shm_open");}
        
    // okresl rozmiar pamieci wspoldzielonej
    if(ftruncate(memory_fd, sizeof(memory_map_t)) < 0){perror("ftruncate");}
        
    // zmapuj pamiec wspoldzielona do przestrzeni adresowej programu
    memory_map_t* memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0);
    if (memory_map == MAP_FAILED){perror("mmap");}
        
    // wyczysc pamiec wspoldzielona
    memset(memory_map, 0, sizeof(memory_map_t));
    memory_map->printers_no = PRINTERS_NO; // ustaw liczbe dostepnych drukarek
    signal(SIGINT, SIGNAL_handler); // zarejestruj handler sygnalu SIGINT, zeby powylaczac wszystko

    // stworz N procesow drukarek
    for (int i = 0; i < PRINTERS_NO; i++){
        // zainicjalizuj semafor dla drukarki i-tej, z gory widac wszystkie drukarki, czesc z nich sie widzi
        sem_init(&memory_map->printers[i].printer_semaphore, 1, 1); // init na 1, i true bo wspoldzielony

        pid_t printer_pid = fork();
        if(printer_pid < 0) {perror("fork");return -1;}
        else if(printer_pid == 0) { //dziecko
            while(is_running) {
                //drukuj jesli drukarka ma drukowac, mozna sciagnac wartosc zmiennej semafora i jak nie > 0 to drukarka ma drukowac
                // if (memory_map->printers[i].printer_state == PRINTING) {
                int val;
                sem_getvalue(&memory_map->printers[i].printer_semaphore, &val);
                if (val <= 0) { //nalezy drukowac
                    printf("Drukarka %d drukuje!\n", i);
                    fflush(stdout);

                    printf("Drukarka %d wydrukowala: ", i);

                    /* print data from the buffer with 1s delay after each character*/
                    for (int letter_idx = 0; letter_idx < strlen(memory_map->printers[i].printer_buffer); letter_idx++) {
                        printf("%c", memory_map->printers[i].printer_buffer[letter_idx]);
                        sleep(1); //1 znaczek na sekunde
                    }

                    /* add newline and flush data to be displayed on the screen */
                    printf("\n");
                    fflush(stdout); // jak wszystko sie wydrukowala to wypisz
                    
                    // zwolnij drukarke, semafor += 1
                    sem_post(&memory_map->printers[i].printer_semaphore);
                }
            }
            exit(0);
        }
    }
    printf("Oczekiwanie na drukarki...\n");
    while(wait(NULL) > 0); //czekaj na wszystkie drukarki
    
    // zamknij semafory drukarek
    for (int i = 0; i < PRINTERS_NO; i++){
        if (sem_destroy(&memory_map->printers[i].printer_semaphore) < 0) {
            perror("sem_destroy");
            return -1;
        }
    }

    // odmapuj pamiec wspoldzielona, zwolnij ja, zamknij deskryptor
    munmap(memory_map, sizeof(memory_map_t));
    shm_unlink(SHARED_MEMORY_DESCRIPTOR_NAME);

    fflush(stdout);
    return 0;
}