#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define PATH_BUFFER_SIZE 1024

int main(int arguments_no , char** arguments){
    if (arguments_no != 2){
        //1 to nazwa programu, reszta to wlasciwe argumenty
        printf("Error: Zla ilosc argumentow ma byc 1!\n");
        return -1;
    }

    char* directory_path = arguments[1];
    size_t dir_path_len = strlen(directory_path);

    if ( dir_path_len+2 > PATH_BUFFER_SIZE ){ //+2 bo /0 i mozliwy dopisek '/'
        printf("Error: Nie mozna przeczytac sciezki za dluga jest !\n");
        return -1;
    }

    DIR* dir = opendir(directory_path);

    if (dir == NULL){
        printf("Error: Nie mozna otworzyc folderu '%s'!?\n", directory_path);
        return -1;
    }

    char path_buffer[PATH_BUFFER_SIZE];
    memcpy(path_buffer, directory_path, dir_path_len);

    // upewnienie sie ze konczy sie / zeby pozniej problemow nie bylo
    if(path_buffer[dir_path_len - 1] != '/'){
        path_buffer[dir_path_len] = '/'; //dopisanie
        path_buffer[dir_path_len+1] = '\0' ;
        dir_path_len++;
    }
    
    unsigned long long total_size = 0; //zmienna do sumowania wszystkich bitow

    struct dirent* dir_file;

    struct stat file_status; //musi tak byc a nie pointer bo to za bufor robi

    while((dir_file = readdir(dir)) != NULL){
        size_t file_name_len = strlen(dir_file->d_name);

        // sprawdzenie czy to sie wgl zmiesci w buforze
        if(dir_path_len + file_name_len + 1 > PATH_BUFFER_SIZE){
            printf("Error: Sciezka do pliku w katalogu sie nie miesci w buforze!\n");
            return -1;
        }
        
        // dopisz nazwe pliku do sciezki
        memcpy(&path_buffer[dir_path_len], dir_file->d_name, file_name_len);
        path_buffer[dir_path_len + file_name_len] = '\0';

        // uzupelnienie zmiennej statusu pliku 
        // stat(path_buffer, &file_status);
        if (stat(path_buffer, &file_status) == -1) {
            printf("Error: Nie mozna zczytac info o pliku\n");
            return -1;
        }
    

        // nie jest katalogiem to jest plikiem
        if(!S_ISDIR(file_status.st_mode)){
            //z polecenia jesli nie jest to wypisac informacje o nim
            // rozmiar i nazwę pliku
            printf("%lu %s\n",file_status.st_size, dir_file->d_name);
            total_size += file_status.st_size;
        }
    }

    printf("total: %llu bytes\n", total_size);

    closedir(dir);
    return 0;
}