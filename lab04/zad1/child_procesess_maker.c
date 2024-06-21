#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char** argv) {
    if(argc != 2) {
        printf("ERROR: zla ilosc argumentow");
        return -1;
    }

    int number_of_processes = strtol(argv[1], NULL, 10);

    if(number_of_processes < 0){
        printf("ERROR: zle podano dane");
        return -1;
    }

    for(int i = 0; i < number_of_processes; i++) {
        pid_t id = fork(); //od tego momentu child proces istnieje

        if(id == 0) {//dla child
            printf("PID rodzica: %d\n", (int)getppid());
            printf("PID dziecka (ten proces) %d\n", (int)getpid());
            exit(0);
        }
    }
    
    while(wait(NULL) > 0); //NULL bo statloc to wskaznik do miejsca w pamieci, gdzie zostanie przekazany status zakonczenia procesu potomnego 

    printf("Wywolano procesow: %d\n", number_of_processes);
    return 0;
}