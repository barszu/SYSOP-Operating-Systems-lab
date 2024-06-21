#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>

#include "specs.h"

volatile bool is_running = true;
int USERS_NO = 10;

void SIGNAL_handler(int signum) {
    is_running = false;
    printf("Zamykanie uzytkownikow...\n");
}

void fill_with_random_string(char* buffer, int len) {
    for(int i = 0; i < len; i++) {
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[len] = '\0';
}


int get_printer_idx(memory_map_t* memory_map) {
    int printer_idx = -1;
    for (int i = 0; i < memory_map->printers_no; i++) {
        int val;
        sem_getvalue(&memory_map->printers[i].printer_semaphore, &val);
        if(val > 0) { //pierwsza lepsza aby drukarka wolna
            printer_idx = i;
            break;
        }
    }

    // kiedy nie znaleziono wolnej drukarki to losujemy
    if(printer_idx == -1) {
        printer_idx = rand() % memory_map->printers_no;
    }
    return printer_idx;
}

void initRandom() {
    // Pobierz identyfikator procesu (PID)
    pid_t pid = getpid();

    // Pobierz aktualny czas
    time_t current_time;
    time(&current_time);

    // Połącz PID i czas, aby uzyskać unikalne ziarno
    unsigned int seed = (unsigned int)(current_time * pid);

    // Inicjalizacja generatora liczb pseudolosowych
    srand(seed);
}

int main() {
    
    //otworz deskryptor do pamieci wspoldzielonej, ale nie tworz jej
    int memory_fd = shm_open(SHARED_MEMORY_DESCRIPTOR_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if(memory_fd < 0){printf("HINT moze drukarki nie sa wlaczone?");perror("shm_open");}
        
    // zmapuj pamiec wspoldzielona do przestrzeni adresowej programu
    memory_map_t* memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0);
    if (memory_map == MAP_FAILED){perror("mmap");}

    // buffer dla dokumentu do wydrukowania
    char user_buffer[MAX_PRINTER_BUFFER_SIZE] = {0};

    // handler sygnalu INT zeby pozamykac wszystko
    signal(SIGINT, SIGNAL_handler);

    // stworzy N procesow uzytkownikow
    for (int i = 0; i < USERS_NO; i++){
        pid_t user_pid = fork(); 
        if (user_pid < 0) {perror("fork"); return -1;}
        else if(user_pid == 0) { //dziecko -> user nadajacy printowanie co chwile
            initRandom();
            while(is_running) {
                sleep(rand() % 3 + 1); //czekaj losowa ilosc czasu zeby chciec drukowac
                
                fill_with_random_string(user_buffer, 10); //stworz losowy dokument dlugosci 10
                int printer_idx = get_printer_idx(memory_map); // znajdz drukarke

                //zmniejsz semafor drukarki, blokuje program jesli drukarka jest jeszcze zajeta
                if(sem_wait(&memory_map->printers[printer_idx].printer_semaphore) < 0){perror("sem_wait");}
                
                // rozpocznij drukowanie
                // skopiuj dokument do bufora drukarki
                memcpy(memory_map->printers[printer_idx].printer_buffer, user_buffer, strlen(user_buffer));

                printf("Uzytkownik %d drukuje wlasnie dokument na drukarce: %d\n", i, printer_idx);
                fflush(stdout);              
            }
            exit(0);         
        }
    }
    printf("Oczekiwanie na uzytkownikow...\n");

    // czekaj na wszystkich uzytkownikow az sie pokoncza
    while(wait(NULL) > 0);

    // zamknij pamiec wspoldzielona
    munmap(memory_map, sizeof(memory_map_t));
    fflush(stdout);
    return 0;
}