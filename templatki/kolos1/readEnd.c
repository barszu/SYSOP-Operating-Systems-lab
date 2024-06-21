#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * Funkcja 'read_end' powinna:
 *  - otworzyc plik o nazwie przekazanej w argumencie
 *    'file_name' w trybie tylko do odczytu,
 *
 *  - przeczytac ostatnie 8 bajtow tego pliku i zapisac
 *    wynik w argumencie 'result'.
 */
void read_end(char *file_name, char *result) {
    // Uzupelnij cialo funkcji read_end zgodnie z
    // komentarzem powyzej

    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Cannot open a file \n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0L, SEEK_END);
    size_t length = ftell(file);
    fseek(file, length - 9, SEEK_SET);

    char buff[sizeof(char) * 8];
    //fread(buff, sizeof(char), 8, file);
    fread(buff, sizeof(*buff), ARRAY_SIZE(buff), file);
    strcpy(result, buff);
    fclose(file);
}


int main(int argc, char *argv[]) {
    int result[2];

    if (argc < 2) return -1;
    read_end(argv[1], (char *) result);
    printf("magic number: %d\n", (result[0] ^ result[1]) % 1000);
    return 0;
}


