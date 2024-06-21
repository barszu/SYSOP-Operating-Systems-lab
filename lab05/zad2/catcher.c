// uruchamiany jako 1, wypisuje swój numer PID i czeka na sygnały SIGUSR1
// Po każdorazowym odebraniu sygnału SIGUSR1 przez program catcher powinno nastąpić potwierdzenie odbioru tego sygnału

// W tym celu, catcher wysyła do sendera sygnał SIGUSR1 informujący o odbiorze sygnału.

#define __USE_POSIX
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

int status = -1;
int status_changes = 0;

void signal_argument_handler(int new_status){ //new_status otrzymany z sygnału
    status_changes++;
    status = new_status;

    if (status < 1 || status > 3) {
        printf("Otrzymano bledny status, dostepne 1 2 3 \n");
        status = -1;
    }
}


void signal_handler(int signal, siginfo_t *signal_info, void *extra){
    // wyciagniecie informacji z struktury signal_info
    int new_status = signal_info->si_int;

    printf("Otrzymano status: %d od procesu z pid: %d\n", new_status, signal_info->si_pid);

    // aktualizacja statusu
    signal_argument_handler(new_status);

    // wyslanie sygnaly potwierdzajacego
    kill(signal_info->si_pid, SIGUSR1);
}

void dummy_loop(){
    for(int i = 1; i <= 100; i++){
        printf("%d, ", i);
    }
    printf("\n");
}

int main() {
    printf("PID Catchera: %d\n", getpid());

    // Zarejestrowanie nowej akcji sygnalu
    struct sigaction action;
    sigemptyset(&action.sa_mask);           // wyczyszczenie maski sygnalow podczas obslugi
    action.sa_sigaction = signal_handler;   // zarejestrowanie funkcji obsługi
    action.sa_flags = SA_SIGINFO;           // ustawienie flagi na SIGINFO, aby funkcja obsługi była wywoływana z trzema argumentami
    
    sigaction(SIGUSR1, &action, NULL);      // zarejestrowanie akcji dla sygnału SIGUSR1

    while(1) {
        switch(status){
            case 1:
                dummy_loop();
                status = -1;
                break;
            case 2:
                printf("Dotychczas status zmienił się %d razy\n", status_changes);
                status = -1;
                break;
            case 3:
                printf("Otrzymano sygnał zakończenia! Kończenie działania...\n");
                return 0;
                break;
        }
    } 

    return 0;
}
