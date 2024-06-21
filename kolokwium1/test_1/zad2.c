#define _XOPEN_SOURCE 700 //zeby sigaction mi dzialal
#include "zad2.h"


void mask()
{
    /*  Zamaskuj sygnał SIGUSR2, tak aby nie docierał on do procesu */

    //jeden sygnal ignorowany z uzyciem maski
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR2, &sa, NULL);


    check_mask();
}

void process()
{
    /*  Stworz nowy process potomny i uruchom w nim program ./check_fork
        W procesie macierzystym:
            1. poczekaj 1 sekundę
            2. wyślij SIGUSR1 do procesu potomnego
            3. poczekaj na zakończenie procesu potomnego */
    pid_t id = fork();
    if (id < 0){
        printf("Fork ERR\n");
        return;
    }
    if (id == 0) {
        //dziecko
        execl("./check_fork", "check_fork", NULL);
        return; //exeec sie nie udal
    } else {
        //rodzic
        sleep(1); 
        kill(id, SIGUSR1); 
        wait(NULL);
        return;
    } 

}

int main()
{
    mask();
    process();

    return 0;
}