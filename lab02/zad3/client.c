// Napisz klienta korzystającego z kodu biblioteki, 
// klient powinien sprawdzać kilka liczb, wykorzystując test_collatz_convergence, 
// tj. po ilu iteracjach wynik zbiegnie się do 1 i wypisać liczbę iteracji na standardowe wyjście. 

#include <stdio.h>

#ifdef DYNAMIC
        #include <dlfcn.h>
#else
        #include "collatz.h"
#endif



void run( int (*function)(int, int)){
    int data[6]  = { 1 , 100 , 300, 69 , 2137 , 21} ;
    int max_iteration = 200 ;
    int iter;
    for (int i=0 ; i<6 ; i++){
        iter = function(data[i], max_iteration);
        printf("n=%d , iter=%d\n" , data[i] , iter);
    } 
}


int main(){
    #ifdef DYNAMIC
        void *handle = dlopen("./libcollatz.so", RTLD_LAZY);
        if(!handle) {
            fprintf(stderr, "Client_dynamic: Error while loading the dynamic library.\n");
            return 1;
        }

        int (*lib_fun)(int, int);
        lib_fun = (int (*)(int, int))dlsym(handle, "test_collatz_convergence");

        if(dlerror() != NULL) {
            fprintf(stderr, "Client_dynamic: Error while getting the symbol from the dynamic library.\n");
            dlclose(handle);
            return 1;
        }

        //uzycie
        run( lib_fun );

        dlclose(handle);
    #else
        run(test_collatz_convergence);
    #endif
    
    return 0;
}