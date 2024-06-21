#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define PIPE "./potok1"

struct pak {
    int i;
    char lit;
};

int main(int lpar, char *tab[]) {
    int w1;
    struct pak ob1;
// 1) utworzyc potok nazwany 'potok1'
//
    mkfifo(PIPE, 0666);

    while (1) {
// 2) wyswietlic obiekt otrzymany z potoku
        w1 = open(PIPE, O_RDONLY);
        read(w1, &ob1, sizeof (ob1));
//
//
        printf("otrzymano: %d %c\n", ob1.i, ob1.lit);
        fflush(stdout);
    }

    return 0;
}