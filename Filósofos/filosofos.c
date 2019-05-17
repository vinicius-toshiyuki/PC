#include <stdio.h>
#include <sys/ioctl.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NF 5 // Número de filósofos

sem_t garfos[NF];

void * filosofo(void *arg);

int main(int argc, char **argv){
	pthread_t thds[NF];
	srand(time(NULL));
	int i;
	for(i = 0; i < NF; i++){
		sem_init(&garfos[i], 0, 1);
		int *id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&thds[i], NULL, filosofo, (void *) id);
	}
	pthread_join(thds[0], NULL);
	for(i = 0; i < NF; i++){
		sem_destroy(&garfos[i]);
	}
	return 0;
}

char *filosofos[] = {
	"A",
	"B",
	"C",
	"D",
	"E"
};
void * filosofo(void *arg){
	int id = *((int *) arg);
	free(arg);
	while(1){
		// Pega dois garfos, da direita e da esquerda, e come
		int r = rand() % 2;
		sem_wait(&garfos[id +  r]);
		sem_wait(&garfos[(id + 1 - r) % NF]);
		printf("%s: vou comer.\n", filosofos[id % (NF + 1)]);
		sleep(2);
		printf("%s: terminei de comer.\n", filosofos[id % (NF + 1)]);
		sem_post(&garfos[id + r]);
		sem_post(&garfos[id + 1 - r]);
		printf("%s: vou pensar.\n", filosofos[id % (NF + 1)]);
		// Ao terminar de comer, libera os garfos e pensa
		sleep(5);
	}
	pthread_exit(NULL);
}
