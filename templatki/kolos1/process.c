#include "zad2.h"
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>

void mask() {
    /*  Zamaskuj sygnał SIGUSR2, tak aby nie docierał on do procesu */
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("SIGPROCMASK");
        exit(EXIT_FAILURE);
    };

    check_mask();
}

void process() {
    /*  Stworz nowy process potomny i uruchom w nim program ./check_fork
        W procesie macierzystym:
            1. poczekaj 1 sekundę
            2. wyślij SIGUSR1 do procesu potomnego
            3. poczekaj na zakończenie procesu potomnego */
    pid_t pid = fork();

    if (pid == 0) {
        execl("check_fork", "check_fork", NULL);
    } else if (pid == -1) {
        perror("PID_ERR");
        exit(EXIT_FAILURE);
    } else {
        sleep(1);
        kill(pid, SIGUSR1);
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    mask();
    process();

    return 0;
}