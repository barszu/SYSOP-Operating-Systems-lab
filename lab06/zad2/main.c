#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h> 
#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>

const char* fifo_child_parent = "my_fifo1"; //0666
const char* fifo_parent_child = "my_fifo2";

double f(double x){
    return 4/(x*x + 1);
}

void create_fifo(const char* fifo_name){ //prubuje utworzyc 2 kolejki fifo
    if (mkfifo(fifo_name, 0666) == -1){
        if (errno != EEXIST){
            printf("mkfifo error\n");
            exit(1);
        }
    }
}

struct calc_args{
    double start;
    double end;
    double h;
    int n;
};

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
    if (argc != 3+1){
        printf("zla liczba argumentow potrzeba 3 -> start end n\n");
        return 1;
    }
    double start = strtod(argv[1],NULL);
    double end = strtod(argv[2],NULL);
    int n = strtol(argv[3],NULL,10); // i na n procesow
    double h = 1.0/(double)n;

    

    create_fifo(fifo_child_parent);
    create_fifo(fifo_parent_child);

    pid_t forked_id = fork() ;
    if (forked_id == 0){
        //child process
        int read_descriptor = open(fifo_parent_child, O_RDONLY); //otworz 1 fifo,
        struct calc_args in_args;
        read(read_descriptor, &in_args, sizeof(struct calc_args)); //czekaj na paczke
        close(read_descriptor);

        double res = calc(in_args.start, in_args.end, in_args.h, in_args.n); //oblicz calke

        int write_descriptor = open(fifo_child_parent, O_WRONLY);
        write(write_descriptor, &res, sizeof(double)); //wyslij wynik
        close(write_descriptor);
        printf("policzono i wyslano wynik\n");
        return 0;
    }
    else {
        // parent process
        int write_descriptor = open(fifo_parent_child, O_WRONLY);//otwrz 2 fifo, 
        struct calc_args in_args = {start,end,h,n}; //zbuduj paczke do wyslania
        write(write_descriptor, &in_args, sizeof(struct calc_args)); //przejslij paczke
        close(write_descriptor);

        int read_descriptor = open(fifo_child_parent, O_RDONLY);
        double res;
        read(read_descriptor, &res, sizeof(double));
        close(read_descriptor);

        //czekaj, i zczytaj wynik double
        printf("obliczono: %f\n",res);
        return 0;
    }
}