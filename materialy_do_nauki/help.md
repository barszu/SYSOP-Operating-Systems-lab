# Makefile
```makefile
cc = gcc 
cflags = -Wall -g

all: target
target: plik.c
	$(cc) $(cflags) -c client.c -o client.o
.PHONY: all clean
```

# Biblioteki

`header.h` <- tylko naglowki funkcji
`lib_code.c` 

użycie: #include "header.h"

## statyczna
```py
static:
	$(CC) $(CGLAGS) -c collatz.c -o collatz.o # kompilacja biblioteki do collatz.o
	ar rcs libcollatz.a collatz.o # tworzenie biblioteki statycznej libcollatz.a

client_static: static client
	$(CC) client.o -o program -L. -lcollatz # łączenie skompilowanych rzeczy w plik wykonywalny
```

## wspoldzielona
```py
shared:
	$(CC) $(CFLAGS) -fPIC -c collatz.c -o collatz.o # (-fPIC position independent code, potrzebny do kodu biblioteki współdzielonej)
	$(CC) $(CFLAGS) -shared -o libcollatz.so collatz.o # tworzenie biblioteki wspoldzielonej libcollatz.so

client_shared: shared
	$(CC) client.c -o program -L. -lcollatz -Wl,-rpath,. # linkowanie z biblioteką (-L. wskazuje na szukanie w obecnym katalogu, a -l podaje bibliotekę)
```

## wspoldzielona dynamiczna 
-> tylko roznice w programie
```py
client_dynamic: shared
	$(CC) $(CFLAGS) client.c -o program -ldl -D DYNAMIC # Linkowanie clienta bez dołączania biblioteki i z dyrektywą DYNAMIC
```
```c
#ifdef DYNAMIC
        #include <dlfcn.h>
#endif

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
#endif
```

# Pliki
```c
#include <stdio.h>

FILE* fopen ( char* file_path, char* mode ); //mode = 'r' | 'w' | 'a'
int fclose ( FILE* file );

//przesuwaja same sobie wskaznik pozycji na pliku
size_t read_bytes = fread ( void* buffer, size_t sizeof_data_type, size_t nmemb, FILE* file);
size_t written_bytes = fwrite ( void* buffer, size_t sizeof_data_type, size_t nmemb, FILE* file);

int fseek ( FILE* file, long bytes_offset, int whence); //whence = 'SEEK_' + 'SET' | 'CUR' | 'END'
int fsetpos (FILE* file, fpos_t* new_pos);
int fgetpos (FILE* file, fpos_t* pos_buffer);
```

```c
int file_descriptor = open(char *file_path, int flags, mode_t mode); //flags= 'O_CREAT | O_WRONLY' ...
int file_descriptor = creat(char *file_path, mode_t mode); //mode= 0666
int close(int fd);

int read(int file_descriptor, void *buffer, size_t bytes_no);
int write(int file_descriptor, void *buffer, size_t bytes_no);

off_t lseek(int fd, off_t bytes_offset, int whence); //whence -||-

```

# Katalogi
```c
DIR* opendir(const char* dir_path);
int closedir(DIR* dirp);

struct dirent* dir_file = readdir(DIR* dirp); //auto next

void rewinddir(DIR* dirp); //set to begining
void seekdir(DIR* dirp, long new_posision);
long pos = telldir(DIR* dirp);

int stat (const char *path, struct stat *buf); //pobranie statystyk (informacji) pliku
int mkdir (const char *path, mode_t mode);
int rmdir (const char *path);

//uzycie
struct dirent* entry;
struct stat statbuf;

entry = readdir(dir)
entry->d_name //nazwa pliku/foderu

stat(file_path , &statbuf)
S_ISDIR(statbuf.st_mode) //czy jest folderem


```

# Procesy
```c
pid_t getpid(void); //this process id
pid_t getppid(void); //this process parent id

pid_t fork(void); // child -> 0 | parent -> child_id

int execl(char *path_to_run_new_program, char *arg0, ... , NULL); //arg0 new program name

pid_t wait(int *child_exit_status_buff); //waiting for anyone | freezing parent
pid_t waitpid(pid_t pid, int *statloc, int options); //pid have special abilities -> info waitpid

void exit(int exit_code); //exit process

```

# Sygnaly
przerwania programowe, nie przechwycony lub nie blokowany konczy proces
SIGINT - Ctrl+C 
SIGTERM - kill
SIGUSR1 , SIGUSR2 - customowe

```c
#define __USE_POSIX
#define _XOPEN_SOURCE 700
#define _POSIX_SOURCE 200809L
#define _POSIX_SOURCE 700

int kill(pid_t pid, int sig); //aka send_signal
int raise(int sig);

void sigaction(int signal, struct sigaction* new_sigaction, struct sigaction* old_sigaction_buff)
void signal(int signum, sighandler_t handler_function) // sighandler_t - func | SIG_DFL | SIG_IGN
// void handler(int signal) {} //funkcja handler

int sigemptyset(sigset_t *set);
int sigaddset(sigset_t *set, int sig);
int sigfillset(sigset_t *set);
int sigdelset(sigset_t *set, int sig);
int sigismember ( sigset_t *signal_set, int sig );

//ustawia maske dla procesu, 
int sigprocmask(int how, const sigset_t *new_set, sigset_t *oldset_buff); //how = SIG_BLOCK | SIG_UNBLOCK | SIG_SETMASK

int sigpending(sigset_t *set); //pobiera czekujace sygnaly
int sigqueue(pid_t pid, int sig, const union sigval value); // wysyla sygnal do procesu z sigval (wrapper)
int sigsuspend(const sigset_t *mask); //Zawiesza bieżący proces i oczekuje na sygnał z zestawu mask.
``` 
```c
//.. dopoki process nie dostanie sygnalu ktory jest nie przechwycony
void pause(); 
unsigned int sleep(unsigned int seconds);

unsigned int alarm (unsigned int sec); //po sec wysle SIGALARM
```

```c
////////////////////////////////////////
// extension

// Zarejestrowanie nowej akcji sygnalu
struct sigaction action;

struct sigaction{
 void (*sa_handler)();/* Wskaźnik do funkcji obsługi sygnału*/
 sigset_t sa_mask; /* Maska sygnałów – czyli zbiór sygnałów blokowanych podczas obsługi
bieżącego sygnału, sygnał przetwarzany jest blokowany domyślnie */
 int sa_flags; /* Nadzoruje obsługę sygnału przez jądro*/
}; 

sigemptyset(&action.sa_mask);           // wyczyszczenie maski sygnalow podczas obslugi
action.sa_sigaction = signal_handler;   // zarejestrowanie funkcji obsługi
action.sa_flags = SA_SIGINFO;           // ustawienie flagi na SIGINFO, aby funkcja obsługi była wywoływana z trzema argumentami

void signal_handler(int signal, siginfo_t *signal_info, void *extra){} //funkcja handler

typedef struct siginfo {
    int      si_signo;    /* Numer sygnału */
    int      si_errno;    /* Wartość błędu (jeśli jest) */
    int      si_code;     /* Kod przyczyny sygnału */
    pid_t    si_pid;      /* PID procesu wysyłającego sygnał */
    uid_t    si_uid;      /* UID użytkownika wysyłającego sygnał */
    void    *si_addr;     /* Adres pamięci powiązany z sygnałem (np. naruszenie ochrony pamięci) */
    int      si_status;   /* Status wyjścia */
    long     si_band;     /* Band event */
    /* Inne pola mogą być zdefiniowane w zależności od systemu */
} siginfo_t;

```



# Potoki

## nienazwane
```c
// int fd[2] = {read write} descriptor
int pipe(int fd[2]);
int close(int fd);
int dup2(int to_set_fd, int where_fd); //where_fd <- to_set_fd , STDIN_FILENO / STDOUT_FILENO


FILE* popen(char* command, char* type); //nowy proces i potok do niego
int pclose(FILE* stream); //Zamyka potok popen i zwraca status zakończenia procesu potomnego.
```

## nazwane
```c
int mkfifo(const char *pathname, mode_t mode);
int mkfifoat(int dirfd, const char *pathname, mode_t mode);
```

