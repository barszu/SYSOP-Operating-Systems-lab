//Makefile
CC = gcc -Wall

all:
    $(CC) -fPIC -c bibl1.c
    $(CC) -shared bibl1.o -o libbibl1.so
    $(CC) zad1.c -o main zad1 -ldl


//zad1.c
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

int main(int l_param, char *wparam[]) {
    int i;
    int tab[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
/*
1) otworz biblioteke
2) przypisz wskaznikom f1 i f2 adresy funkcji z biblioteki sumuj i srednia
3) stworz Makefile kompilujacy biblioteke 'bibl1' ladowana dynamicznie
   oraz kompilujacy ten program
*/
    void* handler = dlopen("./libbibl1.so", RTLD_LAZY);
    int (*f2)(int *tab, int n);
    f2 = dlsym(handler, "sumuj");
    double (*f1)(int *tab, int n);
    f1 = dlsym(handler, "srednia");

    for (i = 0; i < 5; i++) printf("Wynik: %lf, %d\n", f1(tab + i, 20 - i), f2(tab + i, 20 - i));
    return 0;
}



//bibl.c
#include <stdlib.h>
#include <stdio.h>
#include "bibl1.h"

//napisz biblioteke ladowana dynamicznie przez program zawierajaca funkcje:

//1) zliczajaca sume n elementow tablicy tab:
int sumuj(int *tab, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum = tab[1];
    }
    return sum;
}

//2) liczaca srednia n elementow tablicy tab
double srednia(int *tab, int n) {
    int sum = sumuj(tab, n);
    return sum / n;
}
