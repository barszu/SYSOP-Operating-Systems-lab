#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// PTHREAD_MUTEX_INITIALIZER - inicjalizuje muteks w czasie kompilacji | w miejscu deklaracji
// PTHREAD_COND_INITIALIZER - inicjalizuje zmienną warunkową w czasie kompilacji | w miejscu deklaracji

// mutekst i warunek dla Świętego Mikołaja
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;

// muteksy dla reniferów, nie da sie tego zrobic petla for bo krzyczy
// mutexy dla reniferow zeby kazdego z osobna kontrolowac
pthread_mutex_t reindeers_mutexes[9] = {
    PTHREAD_MUTEX_INITIALIZER, // renifer 1
    PTHREAD_MUTEX_INITIALIZER, // renifer 2
    PTHREAD_MUTEX_INITIALIZER, // renifer 3
    PTHREAD_MUTEX_INITIALIZER, // renifer 4
    PTHREAD_MUTEX_INITIALIZER, // renifer 5
    PTHREAD_MUTEX_INITIALIZER, // renifer 6
    PTHREAD_MUTEX_INITIALIZER, // renifer 7
    PTHREAD_MUTEX_INITIALIZER, // renifer 8
    PTHREAD_MUTEX_INITIALIZER  // renifer 9
};

// licznik reniferów, którzy wrócili, mutex czy renifery sa gotowe
int reindeers_ready_no = 0;
pthread_mutex_t reindeers_ready_mutex = PTHREAD_MUTEX_INITIALIZER;

// wątki Świętego Mikołaja i reniferów
pthread_t santa_thread;
pthread_t reindeers_threads[9];


void* reindeer_main(void* arg){ //o id renifera
    int id = *(int*)arg;

    while(1) {
        pthread_mutex_lock(&reindeers_mutexes[id]); //wejdzie do petli po raz kolejny wtedy kiedy mikolaj zwolni mutex

        printf("Renifer z id %d: leci na wakacje \n", id);
        sleep(rand() % 6 + 5); //jedzie na wakacje

        //skonczyl wakacje, jest gotowy

        // zablokowanie muteksu licznika, aby zwiększyć licznik reniferów, którzy wrócili i obudzic mikolaja
        pthread_mutex_lock(&reindeers_ready_mutex); // sekcja krytyczna reniferow
            reindeers_ready_no++; //zwiekszenie licznika reniferow gotowych

            printf("Renifer z id %d gotowy: gotowych wszystkich: %d \n", id, reindeers_ready_no);
            
            if (reindeers_ready_no == 9) { 
                printf("Renifer z id %d budzi Mikołaja \n", id);
                pthread_cond_signal(&santa_cond); //obudz mikołaja
                reindeers_ready_no = 0; //reset licznika
            }
        pthread_mutex_unlock(&reindeers_ready_mutex); // wyjscie z sekcji krytycznej reniferow
    }  

    return NULL;
}

void* santa_main(void* arg) {
    while (1){
        // czekaj, az zostaniesz obudzony
        pthread_cond_wait(&santa_cond, &santa_mutex);
        printf("Mikołaj: budzę się, dostarczam zabawki\n");

        sleep(rand() % 3 + 2); //dostarcza zabawki 2-4 sekundy

        printf("Mikołaj: zwalniam renifery, zasypiam\n");

        for (int j = 0; j < 9; j++) {
            pthread_mutex_unlock(&reindeers_mutexes[j]); //uwolnij renifery
        }
    }
    return NULL;
}

int main() {
    int raindeer_idx[9]; //ponumerowanie reniferow
    pthread_create(&santa_thread, NULL, santa_main, NULL); //watek mikołaja
    for (int i = 0; i < 9; i++) {
        raindeer_idx[i] = i;
        pthread_create(&reindeers_threads[i], NULL, reindeer_main, &raindeer_idx[i]);
    }

    sleep(60); //zeby proces nie zginal na 60s
    return 0;
}