#include "zad3.h"

void readwrite(int pd, size_t block_size);

void createpipe(size_t block_size)
{
    /* Utwórz potok nienazwany */
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        printf("Failed to create pipe");
        return;
    }

    /* Odkomentuj poniższe funkcje zamieniając ... na deskryptory potoku */
    check_pipe(pipe_fd);
    check_write(pipe_fd, block_size, readwrite);
}

void readwrite(int write_pd, size_t block_size)
{
    /* Otworz plik `unix.txt`, czytaj go po `block_size` bajtów
    i w takich `block_size` bajtowych kawałkach pisz do potoku `write_pd`.*/

    FILE *file = fopen("unix.txt", "r");
    if (file == NULL) {
        printf("Failed to open file");
        return;
    }

    char buffer[block_size];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, sizeof(char), block_size, file)) > 0) {
        if (write(write_pd, buffer, bytesRead) == -1) {
            printf("Failed to write to pipe");
            return;
        }
    }

    /* Zamknij plik */
    fclose(file);
}

int main()
{
    srand(42);
    size_t block_size = rand() % 128;
    createpipe(block_size);

    return 0;
}