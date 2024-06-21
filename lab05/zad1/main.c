#define __USE_POSIX
#define _POSIX_SOURCE 200809L

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

// #include <bits/types/sigset_t.h>





void handler(int signal){
    printf("Handler: Otrzymano sygnal: %d \n",signal);
}

// uzycie z dodatkowa flaga
int main(int argc, char** argv){
    if (argc != 2){
        printf("zla liczba argumentow potrzeba 1\n");
        return 1;
    }

    char* program_flag = argv[1];
    // obsluga wdl obcji: none, ignore, handler, mask
    if ( strcmp(program_flag , "none") == 0 ){
        //uzyj defultowego, mozna tutaj nic tez nie robic
        signal(SIGUSR1 , SIG_DFL);

        printf("program sie zakonczy przerwaniem");
    }
    else if (strcmp(program_flag , "ignore" )== 0){
        signal(SIGUSR1, SIG_IGN); 
    }
    else if (strcmp(program_flag , "handler" )== 0){
        signal(SIGUSR1, handler);
    }
    else if (strcmp(program_flag , "mask" )== 0){
        sigset_t signal_set;
        sigemptyset(&signal_set);
        sigaddset(&signal_set , SIGUSR1);

        sigprocmask(SIG_BLOCK, &signal_set, NULL);

        raise(SIGUSR1); // wyslanie do samego siebie jako sprawdzenie

        sigset_t rest_of_signals;
        sigpending(&rest_of_signals);

        printf("sygnal oczekujacy? : %d\n", sigismember(&rest_of_signals, SIGUSR1));
        printf("program zakonczyl dzialanie kod 0\n");
        return 0;
    }
    else {
        // zla flaga
        printf("zla flaga dostepne: none, ignore, handler, mask");
        return 1;
    }

    // wyslanie do samego siebie jako sprawdzenie
    raise(SIGUSR1);
    printf("program zakonczyl dzialanie kod 0\n");
    return 0;
}