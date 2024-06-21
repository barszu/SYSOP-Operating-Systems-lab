#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

double f(double x){
    return 4/(x*x + 1);
}

double calc_chunk(double start , double end , double h){
    return f(start)*h ;
}

double calc(double start, double end, double h, int n){
    double sum = 0.0;
    double x = start;
    for(int i = 0; i < n; i++){
        sum += f(x);
        x += h;
    }
    return sum * h;
}

int main(int argc , char** argv){
    if (argc != 2){
        printf("zla liczba argumentow potrzeba 1 -> n\n");
        return 1;
    }

    int n = strtol(argv[1],NULL,10); // i na n procesow
    double h = 1.0/(double)n;

    double start;
    double end;

    double res = 0.0;
    
    for (int i=0 ; i<n ; i++){
        int file_descriptor[2];
        if (pipe(file_descriptor) == -1){return 1;}
        int read_descriptor = file_descriptor[0];
        int write_descriptor = file_descriptor[1];

        pid_t forked_id = fork() ;
        if (forked_id == 0){
            //child process
            close(read_descriptor);

            start = ((double)i) * h ;
            end = ((double)(i+1)) * h;
            double buff = calc_chunk(start,end,h);

            write(write_descriptor , &buff , sizeof(double));

            close(write_descriptor);
            return 0;
        }
        else {
            // parent process
            close(write_descriptor);
            double partial;
            read(read_descriptor , &partial , sizeof(double));

            res += partial;
            // printf("odebrano %f",partial);
            close(read_descriptor);
        }
    }
    printf("obliczono: %f\n",res);
    return 0;
}