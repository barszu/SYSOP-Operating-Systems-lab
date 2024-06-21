#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sighandler(int sig, siginfo_t *info, void *ucontext) {
    int val = info->si_value.sival_int;
    printf("Sygnał: %d ; Przekazana wartość: %d\n", sig, val);
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;

    //..........
    action.sa_flags = SA_SIGINFO;


    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("SIGPROCMASK - Cannot mask process");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("SIGACTION");
        exit(EXIT_FAILURE);
    }

    int child = fork();
    if (child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc

    } else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        int signal_num = atoi(argv[2]);

        union sigval sigval;
        sigval.sival_int = atoi(argv[1]);

        if (sigqueue(child, signal_num, sigval) == -1) {
            perror("SIGQUEUE - cannot send signal");
            exit(EXIT_FAILURE);
        }
        //int status;
        //wait(&status);
    }
    return 0;
}

