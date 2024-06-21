#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <error.h>

int main() {
    pid_t pid;
    time_t t;
    int status;

    if ((pid = fork()) < 0)
        perror("fork");
    else if (pid == 0) {
        sleep(5);
        exit(5);
    } else
        do {
//  sprawdź status procesu potomka, nie zawieszając przy tym procesu macierzystego
//  1) W przypadku błędu zwróć go.
//  2) W przypadku, gdy proces się  jeszcze nie zakończył:
//     - pobierz aktualną datę.
//     - wypisz "Proces potomny nadal pracuje %data%"
//     - uśpij proces macierzysty na jedną sekundę

            if ((pid = waitpid(pid, &status, WNOHANG)) == -1) {
                perror("waitpid");
            }

            if (pid == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                time(&t);
                printf("Proces potomny nadal pracuje. DATA: %s \n", ctime(&t));
                sleep(1);
            }

            if (WIFEXITED(status)) {
                printf("Proces potomny zakonczył się statusem %d\n", WEXITSTATUS(status));
                return WEXITSTATUS(status);
            } else printf("Proces potomny nie zakonczył się prawidłowo\n");


        } while (pid == 0);
    puts("KONIEC");
    return 0;
}
