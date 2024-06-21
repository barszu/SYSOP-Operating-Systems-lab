#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>





#define MAX_THREAD_NO 8
int thread_no = 3;




typedef struct {
	int thread_idx; // index of thread in array (enumerated)
	//podwojne rzutownie bo z jednym nie dzialalo? i powinien byc segment fault a nie bylo
	char** background; // pointer do background 1D
	char** foreground; // pointer do foreground 1D
} thread_args_t;

void* thread_main(void* arg)
{
	thread_args_t* args = (thread_args_t*)arg;

	while (true) { //nieskonczona petla zeby watek nie umarl
		pause(); //zpasuj dopoki nie zostanie obudzony

		update_part_grid(*(args->foreground), *(args->background), thread_no, args->thread_idx); 
		// printf("\njestem z watku %d\n", args->thread_idx);
	}

	return NULL;
}





void SIGUSR1_handler(int signo) {} 

int main(int argc, char *argv[]) 
{
	if (argc != 1+1) {
		printf("ERR zla ilosc argumentow potrzeba 1 -> thread_no");
		return 1;
	}
	thread_no = atoi(argv[1]);
	if (thread_no < 1 || thread_no > MAX_THREAD_NO) {
		printf("ERR zla ilosc watkow, potrzeba 1 : %d", (MAX_THREAD_NO-1) );
		return 1;
	}


	//sygnaly
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIGUSR1_handler;
	sigaction(SIGUSR1, &sa, NULL);
	
	
	
	
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);


	//tablice do trzymania watkow i ich argumentow
	pthread_t threads[MAX_THREAD_NO];
	thread_args_t threads_args[MAX_THREAD_NO];

	for (int i = 0; i < thread_no; i++) { //tworzenie watkow
		// Assign cells to threads
		threads_args[i].thread_idx = i;

		threads_args[i].foreground = &foreground;
		threads_args[i].background = &background;

		// create threads without any special attributes and with argument structure
		pthread_create(&threads[i], NULL, thread_main, &threads_args[i]);
	}





	while (true)
	{
		draw_grid(foreground);

		// obudz watki i niech zrobia swoje, i sie uspia
		for(int i = 0; i < thread_no; i++) {
			pthread_kill(threads[i], SIGUSR1); //obudz watek
		}


		usleep(500 * 1000);

		// Step simulation
		// update_grid(foreground, background);

		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
