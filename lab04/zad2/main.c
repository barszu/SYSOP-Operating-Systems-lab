#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int global_variable = 0;


int main(int argc, char** argv) {
    if(argc != 2) {
        printf("ERROR: zla ilosc argumentow\n");
        return -1;
    }

    char* program_name = argv[0];
    printf("Program name: %s\n" , program_name);

    char* directory_path = argv[1];

    int local_variable = 0;

    pid_t id = fork();
    if (id == 0){ //kod tylko dla dziecka
        printf("child process\n");
        global_variable++;
        local_variable++;
        printf("child pid = %d, parent pid = %d\n", (int)getpid() , (int)getppid());
        printf("child's local = %d, child's global = %d\n" , local_variable , global_variable );
        int status = execl("/bin/ls" , "ls" , directory_path , NULL);
        if (status != 0){
            printf("ERROR: wywolanie ls nie powiodlo sie\n");
            exit(status);
        }
        exit(0);
    }

    //kod dla 'maina'

    

    int child_exit_status; //buffer
    wait(&child_exit_status);

    child_exit_status = WEXITSTATUS(child_exit_status);

    printf("parent process\n");
    printf("(that) parent pid = %d, child pid = %d\n" , (int)getpid() , id);
    printf("Child exit code: %d\n" , child_exit_status);
    printf("Parent's local = %d, parent's global = %d\n" , local_variable , global_variable);

    return 0;
}