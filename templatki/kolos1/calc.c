#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int is_valid_operator(const char *str) {
    if (strcmp(str, "+") == 0 || strcmp(str, "-") == 0 ||
        strcmp(str, "x") == 0 || strcmp(str, "/") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }
    //utworz proces potomny w ktorym wykonasz program ./calc z parametrami argv[1], argv[2] oraz argv[3]
    //odpowiednio jako pierwszy operand, operacja (+-x/) i drugi operand
    //uzyj tablicowego sposobu przekazywania parametrow do programu


    if ((is_valid_operator(argv[2]) == 0) || (atoi(argv[1]) == 0) || (atoi(argv[3]) == 0)) {
        printf("Wrong arguments \n");
        return (1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        char * args[] = {"calc", argv[1], argv[2], argv[3], NULL};
        execv("calc", args);
    }
    else
    {
        sleep(1);
    }
    return 0;
}
