// wysyla sygnal SIGUSR1
// Sender powinien wysłać kolejny sygnał dopiero po uzyskaniu tego potwierdzenia

#define __USE_POSIX
#define _XOPEN_SOURCE 700

#include <signal.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void handler(int signal) {
    printf("Potwierdzenie odebrania sygnalu %d\n", signal);
}


int main(int argc, char** argv) {
    if(argc != 3) {
        printf("zla ilosc atgumentow uzycie: ./sender <catcher_pid> <signal_argument>\n");
        return -1;
    }

    char* pid_arg = argv[1];
    char* signal_arg = argv[2];

    long catcher_pid = strtol(pid_arg, NULL, 10);
    int signal_argument = strtol(signal_arg, NULL, 10);

    printf("PID sendera: %d\n", getpid());

    signal(SIGUSR1, handler); //zarejestrowanie handlera dla sygnalu SIGUSR1

    // Zablokuj sygnaly, tak aby tylko SIGUSR1 i SIGINT mogły przerwać zawieszenie
    sigset_t mask;
    sigfillset(&mask);

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT); //dla zabicia z terminala


    union sigval value = {signal_argument}; //argument przekazywany w sygnale wiec trzeba zmienic typ

    // Wyslij sygnal do catchera z argumentem 
    sigqueue(catcher_pid, SIGUSR1, value);
    printf("sygnal wyslany z argumentem: %d\n", signal_argument);

    // oczekuj na sygnal
    sigsuspend(&mask);

    printf("Odebrano potwierdzenie, koniec programu\n");
    return 0;
}



