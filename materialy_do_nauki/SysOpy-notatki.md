
## Lab1 - Makefile

```
cc = gcc 
cflags = -Wall -o

all: target
target: plik.c
	$(cc) $(cflags) plik plik.c -g
clean:
	-rm countdown

.PHONY: all clean
```
- **-g** dostarcza ekstra informacje dla debuggera
- **.PHONY** zawiera targety które nie generują plików (bezpośrednio)
- Możliwe **cflags** to:
>	**-Wall (wyświetla warningi) 
>	-Wextra (dodatkowe warningi)
>	-O0 (optymalizacja na poziomie zerowym czyli jej brak)
>	-o (mówi do pliku o jakiej nazwie ma się skompilować plik.c)** 
## Lab2- Biblioteki

---

### Budowa
Biblioteka w C składa się z kodu źródłowego *jakis_plik.c* oraz nagłówka *nagłówek.h* zawierającego sygnatury funkcji

```c
int collatz_conjecture(int input);
int test_collatz_convergence(int input, int max_iter);
```
Customowe biblioteki importujemy tak:
```
#include "collatz.h"
```
### Biblioteki statyczne
- Zaciągają wszystkie funkcje w bibliotece
- Są łączone z programem podczas **kompilacji**
- Każdy egzemplarz programu ma swoją kopię kodu biblioteki
##### *Sposób kompilacji*
```
static:
	$(cc) -c collatz.c
	# kompilacja biblioteki do collatz.o
	ar rcs libcollatz.a collatz.o
	# tworzenie biblioteki statycznej libcollatz.a
	$(cc) -c client.c
	# kompilacja client.c do client.o
	$(cc) -g client.o libcollatz.a -o client_static $(cflags)
	# łączenie skompilowanych rzeczy w plik wykonywalny
```

### Biblioteki współdzielone
- Mogą być używane przez wiele programów naraz
- Są łączone z programem podczas **uruchomienia**
- programy zamiast kopii kodu biblioteki biorą tylko odwołanie do niego
##### *Sposób kompilacji*
```
shared:
	$(cc) -fPIC -c collatz.c
	# kompilacja collatz.c z kodem o adresach niezależnych
	$(cc) -shared -o libcollatz.so collatz.o
	$(cc) $(cflags) -o client_shared client.c -L. -lcollatz -g
	# linkowanie pliku z biblioteką (-L. wskazuje na szukanie w obecnym
	katalogu, a -l podaje bibliotekę)
```
### Biblioteki dynamiczne
- Są ładowane i wykonywane podczas uruchamiania programu tak jak biblioteki współdzielone
- Można je ładować i rozładowywać dynamicznie podczas działania programu
##### *Sposób kompilacji*
```
dynamic:
	$(cc) -fPIC -c collatz.c
	# kompilacja collatz.c z kodem o adresach niezaleznych
	$(cc) -shared -o libcollatz.so collatz.o
	$(cc) -DDYNAMIC -o client_dynamic client.c -g
	# Linkowanie clienta bez dołączania biblioteki i z dyrektywą DYNAMIC
```

Dyrektywę preprocesora przekazujemy wtedy, gdy od niej zależy to, co wykonuje się w kodzie

```c
#ifdef DYNAMIC
	void *handle;
	int (*test_collatz_convergence)(int, int); //deklaracja wskaźnika do funkcji
	char *error;
	
	  
	handle = dlopen("./libcollatz.so", RTLD_LAZY); //leniwe ładowanie zawartości biblioteki, tylko jak jest potrzebna
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}
	
	  
	test_collatz_convergence = dlsym(handle, "test_collatz_convergence");
	// wydobycie funkcji; od teraz można sobie jej po prostu używać
	if ((error = dlerror()) != NULL) {
	fprintf(stderr, "%s\n", error);
	dlclose(handle);
		return 1;
	}
	dlclose(handle);
	
	return 0;
#else
		...
#endif
```

---

## Lab3- System plików

argumenty do funkcji main przekazuje się tak:

```c
int main(int argc, char const *argv[])
{
if (argc != 3)
{
printf("Podano niepoprawne argumenty!");
return -1;
}
```
argc to liczba argumentów, argv to argumenty, pierwszy to nazwa pliku

### Pliki
funkcje do obsługi plików **(są z biblioteki standardowej)**:

```c
FILE * fopen ( const char * filename, const char * mode );
```
Otwiera plik o podanej nazwie w określonym trybie i zwraca wskaźnik do struktury `FILE`.
- `filename`: Nazwa pliku do otwarcia.
- `mode`: Tryb otwarcia pliku (np. "r" - tylko do odczytu, "w" - do zapisu, "a" - do dopisywania).

```c
size_t fread ( void * ptr, size_t size, size_t count, FILE * file);
```
Czyta dane z pliku i zapisuje je do bufora.
- `ptr`: Wskaźnik do bufora, gdzie mają być zapisane dane.
- `size`: Rozmiar pojedynczego elementu w bajtach.
- `count`: Liczba elementów do odczytu.
- `file`: Wskaźnik do struktury `FILE`.

```c
int fseek ( FILE * file, long int offset, int mode);
```
Przesuwa wskaźnik pozycji w pliku.
- `file`: Wskaźnik do struktury `FILE`.
- `offset`: Przesunięcie w bajtach.
- `mode`: Sposób przesunięcia (np. `SEEK_SET` - od początku, `SEEK_CUR` - bieżąca pozycja, `SEEK_END` - koniec pliku).

```c
size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * file);
```
Zapisuje dane z bufora do pliku.
- `ptr`: Wskaźnik do bufora z danymi do zapisania.
- `size`: Rozmiar pojedynczego elementu w bajtach.
- `count`: Liczba elementów do zapisania.
- `file`: Wskaźnik do struktury `FILE`.

```c
int fsetpos (FILE* file, fpos_t* pos);
```
Ustawia wskaźnik pozycji w pliku na określonej pozycji.
- `file`: Wskaźnik do struktury `FILE`.
- `pos`: Wskaźnik do struktury `fpos_t`, która przechowuje pozycję w pliku.

```c
int fgetpos (FILE* file, fpos_t* pos);
```
Pobiera bieżącą pozycję wskaźnika w pliku.
- `file`: Wskaźnik do struktury `FILE`.
- `pos`: Wskaźnik do struktury `fpos_t`, gdzie zostanie zapisana pozycja.

```c
int fclose ( FILE * stream );
```
Zamyka plik.
- `stream`: Wskaźnik do struktury `FILE`.

Przykładowe wykorzystanie kodu:
```c 
// kod odwracający zawartość pliku bo bajcie albo po blokach bajtów

FILE *input = fopen(argv[1], "r");
FILE *output = fopen(argv[2], "w");
if (input == NULL || output == NULL){
	printf("Wypstąpił błąd podczas ładowania jednego z plikow!");
	return -1;
}
#ifdef BYTE_CHUNKS
	fseek(input, 0, SEEK_END);
	long bytesWritten = 0, size = ftell(input), codeChunkSize ;
	char codeChunk[1024], temp;
	while(bytesWritten < size){
	if((size - bytesWritten) > 1024){
	codeChunkSize = 1024;
	}
	else{
	codeChunkSize = size - bytesWritten;
	}
	fseek(input, -codeChunkSize, SEEK_CUR);
	size_t elementsRead = fread(codeChunk, sizeof(char), codeChunkSize, input);
	for(int i = 0; i < elementsRead/2; i++){
	temp = codeChunk[i];
	codeChunk[i] = codeChunk[elementsRead - 1 - i];
	codeChunk[elementsRead - 1 - i] = temp;
	} 
	fwrite(codeChunk, sizeof(char), elementsRead, output);
	fseek(input, -1024, SEEK_CUR);
	bytesWritten += elementsRead;
	}
#else
	fseek(input, -1, SEEK_END);
	long size = ftell(input) + 1;
	for (int i = size; i > 0; i--)
	{
	fprintf(output, "%c", getc(input));
	fseek(input, -2, SEEK_CUR);
	}
	#endif
	fclose(input);
	fclose(output);
	return 0;
}
```

### Katalogi

Funkcje do obsługi katalogów:

```c
DIR* opendir(const char* dirname);
```
Otwiera katalog o podanej nazwie i zwraca wskaźnik do struktury `DIR`.
- `dirname`: Nazwa katalogu do otwarcia.

```c
int closedir(DIR* dirp);
```
Zamyka katalog.
- `dirp`: Wskaźnik do struktury `DIR`.

```c
struct dirent* readdir(DIR* dirp);
```
Pobiera następny wpis (plik lub katalog) z otwartego katalogu.
- `dirp`: Wskaźnik do struktury `DIR`.

```c
void rewinddir(DIR* dirp);
```
Przewija katalog do początku.
- `dirp`: Wskaźnik do struktury `DIR`.

```c
void seekdir(DIR* dirp, long int loc);
```
Przesuwa pozycję w otwartym katalogu.
- `dirp`: Wskaźnik do struktury `DIR`.
- `loc`: Nowa pozycja w katalogu.

```c
#include <sys/stat.h>
int stat (const char *path, struct stat *buf);
```
Pobiera informacje o pliku lub katalogu.
- `path`: Ścieżka do pliku lub katalogu.
- `buf`: Wskaźnik do struktury `stat`, gdzie zostaną zapisane informacje.

```c
#include <sys/types.h>
int mkdir (const char *path, mode_t mode);
```
Tworzy nowy katalog.
- `path`: Ścieżka do nowego katalogu.
- `mode`: Tryb dostępu do nowego katalogu.

```c
int rmdir (const char *path);
```
Usuwa katalog.
- `path`: Ścieżka do katalogu do usunięcia.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char const *argv[]){
if (argc != 2){
	printf("Podano niepoprawne argumenty!");
	return -1;
}

DIR *dir = opendir(argv[1]);
if (dir == NULL){
	printf("Błąd podczas otwierania folderu!");
	return -1;
}

struct dirent *entry;
struct stat buf;
long long totalSize = 0;
while ((entry = readdir(dir)) != NULL){
	if (entry->d_type == DT_DIR) continue;
	if (stat(entry->d_name, &buf) == -1){
		perror("Error getting file status");
		closedir(dir);
		return 0;
	}
	if (!S_ISDIR(buf.st_mode)){
		printf("%s - %lld bytes\n", entry->d_name, (long  long)buf.st_size);
		
		totalSize += buf.st_size;
		}
}
printf("Total size: %lld\n", totalSize);
return 0;
}
```

---

## Lab 4- Procesy

**Proces** jest pojedynczą instancją wykonującego się programu. Wszystkie procesy w UNIXie tworzą się funkcją fork() poza pierwszym o PID=1. Procesy mają swoje procesy potomne i macierzyste

### Zarządzanie procesami
Funkcje do zarządzania procesami **(są z biblioteki standardowej)**:

```c
pid_t getpid(void);
```
Zwraca identyfikator procesu (PID) bieżącego procesu.

```c
pid_t getppid(void);
```
Zwraca identyfikator procesu (PID) rodzica bieżącego procesu.

```c
pid_t fork(void);
```
Tworzy nowy proces poprzez kopiowanie bieżącego procesu.
- Zwraca: 
  - W procesie potomnym: `0`.
  - W procesie rodzicielskim: PID potomka.
  - W przypadku błędu: `-1`.

```c
int execl(char const *path, char const *arg0, ...);
```
Zamienia bieżący proces na nowy proces, wykonując program z określoną ścieżką.
- `path`: Ścieżka do wykonywanego programu.
- `arg0`: Pierwszy argument dla wykonywanego programu.
- Zwraca: `-1` w przypadku błędu, w innym przypadku nie powraca.

```c
pid_t wait(int *statloc);
```
Czeka na zakończenie dowolnego procesu potomnego i zwraca jego PID.
- `statloc`: Wskaźnik do miejsca, gdzie zostanie zapisany status zakończenia procesu.
- Zwraca: PID procesu potomnego, który zakończył działanie.

```c
pid_t waitpid(pid_t pid, int *statloc, int options);
```
Czeka na zakończenie określonego procesu potomnego.
- `pid`: PID procesu potomnego, na który czekamy (`-1` czeka na dowolnego potomka, `0` czeka na dowolnego potomka z takim samym grupowym ID procesu, dodatnia wartość oczekuje na określonego potomka).
- `statloc`: Wskaźnik do miejsca, gdzie zostanie zapisany status zakończenia procesu.
- `options`: Dodatkowe opcje (np. `WNOHANG` - nie czeka, jeśli nie ma zakończonych procesów).
- Zwraca: PID procesu potomnego, który zakończył działanie.

```c
void exit(int status);
```
Kończy działanie bieżącego procesu i zwraca status wyjścia.
- `status`: Status wyjścia procesu.

```c
void _exit(int status);
```

Przykładowy kod 1:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
if (argc != 2){
		printf("podano złą liczbę argumentow!");
	}
	int id;
	int process_no = strtol(argv[1],NULL,10);
	for(int i=0; i<process_no; i++){
		id = fork();
		if(id==0){
			printf("id: %d ", (int)getpid());
			printf("id: %d", (int)getppid());
			printf("\n");
			exit(0);
		}
	}
	while(wait(NULL)>0){};
	printf("argv[1]: %s\n", argv[1]);
	return 0;
}
```

Przykładowy kod 2:
```c
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

int global = 0;

int main(int argc, char *argv[]){
	if (argc != 2){
		printf("podano złą liczbę argumentow!");
		return 1;
	}
	printf("%s",argv[0]);
	int local = 0;
	pid_t pid = fork();
	// kod dziecka
	if (pid == 0) {
		printf("child process\n");
		local++;
		global++;
		printf("child pid = %d, parent pid = %d \n", getpid(), getppid());
		printf("child's local = %d, child's global = %d \n", local, global);
		// Execl przyjmuje listę argumentów
		int status = execl("/bin/ls", "ls", "-l", argv[1], NULL);
		exit(status);
	}
	int status = 0;
	wait(&status);
	int childExitStatus = WEXITSTATUS(status);
	printf("parent process\n");
	printf("parent pid = %d, child pid = %d \n", getpid(), pid);
	printf("Child exit code: %d \n", childExitStatus);
	printf("parent's local = %d, parent's global = %d \n", local, global);
	return childExitStatus;
```

---

## Lab 5- Sygnały

**Sygnały** to swego rodzaju programowe przerwania, to znaczy zdarzenia, które mogą nastąpić w dowolnym
momencie działania procesu, niezależnie od tego, co dany proces aktualnie robi.
Sygnały są asynchroniczne. Umożliwiają komunikację między procesami.

**Maska sygnału** to zbiór sygnałów który powstrzymuje proces przed tym by sygnały dotarły bezpośrednio do niego

| Nazwa   | Numer | Znaczenie                                                        | Czynność domyślna           |
| ------- | ----- | ---------------------------------------------------------------- | --------------------------- |
| SIGHUP  | 1     | Przerwanie łączności z terminalem.                               | Zakończenie                 |
| SIGINT  | 2     | Terminalowe przerwanie (Ctrl+C)                                  | Zakończenie                 |
| SIGQUIT | 3     | Terminalowe zakończenie (Ctrl+\)                                 | Zrzut pamięci i zakończenie |
| SIGILL  | 4     | Nielegalna instrukcja sprzętowa                                  | Zrzut pamięci i zakończenie |
| SIGABRT | 6     | Przerwanie procesu. Wysyłany przez funkcję abort()               | Zrzut pamięci i zakończenie |
| SIGFPE  | 8     | Wyjątek arytmetyczny (np. dzielenie przez 0)                     | Zrzut pamięci i zakończenie |
| SIGKILL | 9     | Unicestwienie (nie da się przechwycić ani zignorować)            | Zakończenie                 |
| SIGSEGV | 11    | Niepoprawne wskazanie pamięci                                    | Zrzut pamięci i zakończenie |
| SIGPIPE | 13    | Zapis do potoku zamkniętego z jednej strony                      | Ignorowany                  |
| SIGALRM | 14    | Pobudka (upłynął czas ustawiony funkcją alarm() lub setitimer()) | Ignorowany                  |
| SIGTERM | 15    | Zakończenie programowe (domyślny sygnał polecenia kill)          | Zakończenie                 |
| SIGCHLD | 17    | Zakończenie procesu potomnego                                    | Ignorowany                  |
| SIGSTOP | 19    | Zatrzymanie (nie da się przechwycić ani zignorować)              | Zatrzymanie                 |
| SIGCONT | 18    | Kontynuacja wstrzymanego procesu                                 | Ignorowany                  |
| SIGTSTP | 20    | Terminalowe zatrzymanie (Ctrl+Z lub Ctrl+Y)                      | Zatrzymanie                 |
| SIGTTIN | 21    | Czytanie z terminala przez proces drugoplanowy                   | Zatrzymanie                 |
| SIFTTOU | 22    | Pisanie do terminala przez proces drugoplanowy                   | Zatrzymanie                 |
| SIGUSR1 | 10    | Sygnał zdefiniowany przez użytkownika                            | -                           |
| SIGUSR1 | 12    | Sygnał zdefiniowany przez użytkownika                            | -                           |

### Zarządzanie sygnałami
Funkcje do zarządzania sygnałami:

```c
int kill(pid_t pid, int sig);
```
Wysyła sygnał `sig` do procesu o identyfikatorze `pid`.
- `pid`: Identyfikator procesu.
- `sig`: Sygnał do wysłania.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int raise(int sig);
```
Generuje sygnał `sig` w bieżącym procesie.
- `sig`: Sygnał do wygenerowania.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigemptyset(sigset_t *set);
```
Inicjuje zbiór sygnałów jako pusty.
- `set`: Wskaźnik do zbioru sygnałów.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigaddset(sigset_t *set, int signum);
```
Dodaje sygnał `signum` do zbioru sygnałów.
- `set`: Wskaźnik do zbioru sygnałów.
- `signum`: Numer sygnału.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```
Ustawia maskę dla jakiegoś procesu
- `how`: Sposób zmiany blokady sygnałów (SIG_BLOCK,SIG_UNBLOCK,SIG_SETMASK)
- `set`: Wskaźnik do zbioru sygnałów.
- `oldset`: Wskaźnik do poprzedniego zbioru sygnałów (opcjonalny).
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigpending(sigset_t *set);
```
Pobiera bieżący zestaw oczekujących sygnałów.
- `set`: Wskaźnik do zbioru sygnałów.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigqueue(pid_t pid, int sig, const union sigval value);
```
Wysyła sygnał `sig` do procesu `pid` z wartością `value`.
- `pid`: Identyfikator procesu.
- `sig`: Sygnał do wysłania.
- `value`: Wartość do przekazania wraz z sygnałem.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigfillset(sigset_t *set);
```
Inicjuje zbiór sygnałów zawierający wszystkie sygnały w systemie.
- `set`: Wskaźnik do zbioru sygnałów.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigdelset(sigset_t *set, int signum);
```
Usuwa sygnał `signum` ze zbioru sygnałów.
- `set`: Wskaźnik do zbioru sygnałów.
- `signum`: Numer sygnału.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int sigsuspend(const sigset_t *mask);
```
Zawiesza bieżący proces i oczekuje na sygnał z zestawu mask.
- `mask`: Zestaw blokowanych sygnałów.
- Zwraca: `-1`, zawsze. Proces zostanie wznowiony po otrzymaniu sygnału.

```c
typedef void (*sighandler_t)(int);
sighandler_t sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
Ustawia obsługę sygnału `signum` na `act`.
- `signum`: Numer sygnału.
- `act`: Wskaźnik do struktury `sigaction`, która definiuje nową obsługę sygnału.
- `oldact`: Opcjonalny wskaźnik, do którego zostanie zapisana poprzednia obsługa sygnału.
- Zwraca: Wskaźnik do poprzedniej obsługi sygnału w przypadku sukcesu, `SIG_ERR

### Przykładowy kod:
```c
#define _POSIX_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
// #include <bits/types/sigset_t.h>

void handle_signal(int sig){
	printf("Handler reaguje na sygnał");
};

int main(int argc, char *argv[]){
	if (argc != 2){
		printf("podano złą liczbę argumentow!");
		return 1;
	}
	if(strcmp(argv[1],"none")==0){}
	else if(strcmp(argv[1],"ignore")==0){
		signal(SIGUSR1,SIG_IGN);
		raise(SIGUSR1);
	}
	else if(strcmp(argv[1],"handler")==0){
		signal(SIGUSR1, handle_signal);
		raise(SIGUSR1);
	}
	else if(strcmp(argv[1],"mask")==0){
		sigset_t newmask;
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGUSR1);
		sigprocmask(SIG_SETMASK, &newmask, NULL);
		raise(SIGUSR1);
		sigset_t pending;
		sigpending(&pending);
		printf("Czy sygnał jest oczekujący? %i\n", sigismember(&pending, SIGUSR1));
	}
	else{
		printf("Podano niewłaściwy argument\n");
		return 1;
	}
	return 0;
}
```

### Przykładowy kod 2 (sender)
```c
#define _XOPEN_SOURCE 700
// #define _POSIX_SOURCE 200809L
// #include <bits/types/sigset_t.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig) {
printf("Otrzymano potwierdzenie\n");
}

int main(int argc, char** argv) {
	if(argc < 3) {
		printf("Nieprawidlowa liczba argumentów");
		return -1;
	}
	printf("Czy sygnał jest oczekujący? %i\n", sigismember(&pending, SIGUSR1));
	printf("id sendera: %d\n", getpid());
	signal(SIGUSR1, handler);
	long signalId = strtol(argv[1], NULL, 10);
	long sig_argument = strtol(argv[2], NULL, 10);
	union sigval sig_val = {sig_argument};
	sigqueue(signalId, SIGUSR1, sig_val);
	printf("Signal sent with argument: %ld\n", sig_argument);
	sigset_t newMask;
	sigfillset(&newMask);
	sigdelset(&newMask, SIGUSR1);
	sigdelset(&newMask, SIGINT);
	sigsuspend(&newMask);
return 0;
}
```

### Przykładowy kod 3 (catcher)
```c
// #define _POSIX_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
// #include <bits/types/sigset_t.h>

// zmienna widziana przez inny program
volatile int status = -1;
volatile int status_changes = 0;

void SIGUSR1_action(int signo, siginfo_t *info, void *extra){
	int int_val = info->si_int;
	printf("Otrzymano status %d od %d\n", int_val, info->si_pid);
	status_changes++;
	status = int_val;
	kill(info->si_pid, SIGUSR1);
}

int main() {
	printf("id catchera: %d\n", getpid());
	struct sigaction action;
	action.sa_sigaction = SIGUSR1_action;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGUSR1, &action, NULL);
	while(1) {
		switch(status){
			case 1:
				for(int i = 1; i <= 100; i++){
					printf("%i, ", i);
				}
				printf("\n");
				status = -1;
				break;
			case 2:
				printf("Liczba zmian statusu %d\n", status_changes);
				status = -1;
				break;
			case 3:
				printf("Otrzymano sygnał by skończyć!\n");
				exit(0);
				break;
			default:
				break;
		}
	}
	return 0;
}
```

---

## Lab 6- Potoki
**Potok (pipe)** to taki tunel do wymiany danych między dwoma programami. Ma dwa końce (na każdym z nich jest wlot i wylot jako dwie osobne rzeczy). Fizycznie jest plikiem

### Potoki nienazwane
- Umożliwiają transport danych tylko w jedną stronę
- Można nimi transportować dane tylko między procesami o wspólnym przodku (który musi stworzyć potok)

### Potoki nazwane
- mają plik widoczny na dysku przez co dowolne procesy mogą brać udział w wyniku wymiany danych za ich pośrednictwem

***Najpierw użyj forka, a potem dopiero pipe!***

*fd[0] - końcowka do odczytu*
*fd[1] - końcówka do zapisu*


### Potoki nienazwane

```c
int pipe(int fd[2]);
```
Tworzy nienazwany kanał komunikacyjny i zwraca dwa deskryptory plików w tablicy `fd`.
- `fd`: Tablica dwóch deskryptorów plików: `fd[0]` dla odczytu, `fd[1]` dla zapisu.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int close(int fd);
```
Zamyka deskryptor pliku `fd`.
- `fd`: Deskryptor pliku do zamknięcia.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int dup2(int oldfd, int newfd);
```
Duplikuje deskryptor pliku `oldfd` na `newfd`, zastępując go, jeśli jest otwarty.
- `oldfd`: Istniejący deskryptor pliku.
- `newfd`: Nowy deskryptor pliku.
- Zwraca: Nowy deskryptor pliku (`newfd`) w przypadku sukcesu, `-1` w przypadku błędu.

```c
FILE* popen(const char* command, const char* type);
```
Tworzy potok popen do wywołania polecenia `command` jako proces potomny.
- `command`: Polecenie do wykonania.
- `type`: Tryb dostępu (`"r"` dla czytania, `"w"` dla zapisu).
- Zwraca: Strumień plikowy skojarzony z procesem potomnym w przypadku sukcesu, `NULL` w przypadku błędu.

```c
int pclose(FILE* stream);
```
Zamyka potok popen i zwraca status zakończenia procesu potomnego.
- `stream`: Strumień plikowy uzyskany z `popen`.
- Zwraca: Status zakończenia procesu potomnego.

### Potoki nazwane

```c
int mkfifo(const char *pathname, mode_t mode);
```
Tworzy nazwany potok FIFO o nazwie `pathname`.
- `pathname`: Ścieżka do potoku FIFO.
- `mode`: Uprawnienia dostępu do potoku FIFO.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int mknod(const char *pathname, mode_t mode, dev_t dev);
```
Tworzy nowy plik specjalny o nazwie `pathname`.
- `pathname`: Ścieżka do nowego pliku specjalnego.
- `mode`: Uprawnienia dostępu do pliku.
- `dev`: Numer urządzenia.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int mkfifoat(int dirfd, const char *pathname, mode_t mode);
```
Tworzy nazwany potok FIFO o nazwie `pathname` w danym katalogu.
- `dirfd`: Deskryptor otwartego katalogu, w którym ma zostać utworzony potok FIFO.
- `pathname`: Ścieżka do potoku FIFO.
- `mode`: Uprawnienia dostępu do potoku FIFO.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.

```c
int mknodat(int dirfd, const char *pathname, mode_t mode, dev_t dev);
```
Tworzy nowy plik specjalny o nazwie `pathname` w danym katalogu.
- `dirfd`: Deskryptor otwartego katalogu, w którym ma zostać utworzony plik specjalny.
- `pathname`: Ścieżka do nowego pliku specjalnego.
- `mode`: Uprawnienia dostępu do pliku.
- `dev`: Numer urządzenia.
- Zwraca: `0` w przypadku sukcesu, `-1` w przypadku błędu.
