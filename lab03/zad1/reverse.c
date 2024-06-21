#include <stdio.h>
// dziala to tylko dla 2 roznych plikow tekstowych

// struktura file (rozna od OS)
// -deskryptor
// -tryb otwarcia
// -pozycja wskaznika pliku
// -flagi pliku, bufor danych ...

#if defined(BYTES_ONLY)
    #define BUFFER_SIZE 1
#elif !defined(BYTES_ONLY)
    #define BUFFER_SIZE 1024
#endif



typedef char Byte;

// Funkcja zwracająca mniejszą z dwóch liczb
int min(size_t a, size_t b) {
    return (a < b) ? a : b;
}

void reverseArray(Byte* arr, size_t size) {
    long start = 0;
    long end = size - 1;

    // Zamiana kolejności elementów tablicy
    while (start < end) {
        // Zamiana wartości elementów na końcach tablicy
        Byte temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;

        // Przesunięcie wskaźników wewnątrz tablicy
        start++;
        end--;
    }
}

void reverse(FILE* input_file, FILE* output_file){
    //przesun 'wewnetrzny ptr' input_file na koniec pliku
    fseek(input_file, 0, SEEK_END);
    long bytes_left = ftell(input_file);

    Byte buffer[BUFFER_SIZE];
    size_t bytes_to_read;
    size_t read_bytes;
    // fseek(input_file, -1, SEEK_END); //dodatek
    while(bytes_left > 0){
        bytes_to_read = min(BUFFER_SIZE , bytes_left); //chunk size ktory bedzie wczytany

        fseek(input_file, -bytes_to_read , SEEK_CUR);

        //powinnno to byc to samo co bytes_to_read ale dla bezpieczenstwa
        read_bytes = fread(buffer, sizeof(Byte) , bytes_to_read , input_file );

        //odwroc tu buffer
        reverseArray(buffer , read_bytes);

        fwrite(buffer, sizeof(Byte) , read_bytes , output_file );
        // printf("writing '%c'\n",buffer[0]);

        fseek(input_file , -bytes_to_read , SEEK_CUR); //przesun o jeden wstecz
        bytes_left = bytes_left - read_bytes; 
    } 
}



int check_file(FILE* file , char* file_name){ //-> bool
    if (file == NULL){
        printf("Error: Nie mozna otworzyc pliku '%s'!?\n", file_name);
        return 0;
    }
    return 1 ;
}

int main(int arguments_no , char** arguments) {
    if (arguments_no != 3){
        //1 to nazwa programu, reszta to wlasciwe argumenty
        printf("Zla ilosc argumentow ma byc 2!\n");
        return -1;
    }

    // printf("hello world\n");
    FILE *input_file = fopen(arguments[1], "r");
    FILE *output_file = fopen(arguments[2], "w");

    if (!(check_file(input_file , arguments[1]) && check_file(output_file , arguments[2]))){
        return -1;
    }

    reverse(input_file, output_file);

    fclose(input_file);
    fclose(output_file);

    printf("Plik został odwrocony!\n");

    return 0;
}