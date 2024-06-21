#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    pid_t pid;
    switch (pid = fork()) {
        case 0:
            sleep(3);
            printf("Potomek: %d kończy działanie\n", getpid());
            exit(EXIT_SUCCESS);
        case -1:
            perror("FORK");
            exit(EXIT_FAILURE);
        default: {
            /* Wyślij do potomka sygnał SIGINT i obsłuż błąd */
            /* Czekaj na zakończenie potomka i pobierz status.
               1) Jeśli potomek zakończył się normalnie (przez exit), wypisz informację wraz wartością statusu i jego PID.
               2) Jeśli potomek zakończył się sygnałem, zwróć taką informację (wartość sygnału) */
            if(kill(pid, SIGINT) == -1){
                perror("kill");
                exit(EXIT_FAILURE);
            };

            int status;
            waitpid(pid, &status, WNOHANG);

            if (WIFSIGNALED(status)) {
                return WTERMSIG(status);
            } else if (WIFEXITED(status)) {
                printf("wartość statusu: %d , pid: %d \n" , WEXITSTATUS(status), pid);
            }
            return 1;
        }
    }
}