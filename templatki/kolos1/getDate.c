#include <stdio.h>
#include <sys/wait.h>

#define BUF_SIZE 256

/*
 * Funkcja 'get_date' powinna
 * 1) wywołać polecenie 'date'
 * 2) z jego wyjścia standardowego wczytać linijkę do bufora 'buf'
 * 3) poczekać na zakończenie utworzonego procesu i zwrócić jego kod wyjścia
 */
void get_date(char *buf, int size) {
    FILE *file = popen("date", "r");
    fgets(buf, size * sizeof(char), file);

    pclose(file);

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        printf("proces zakończył się ze statusem: %d \n", WEXITSTATUS(status));
    } else {
        printf("proces zakończył się niepoprawnie \n");
    }
}

int main(void) {
    char buf[BUF_SIZE];
    get_date(buf, BUF_SIZE);

    if (wait(NULL) != -1) {
        printf("Child not reaped\n");
    }
    printf("Date: %s", buf);
    return 0;
}


