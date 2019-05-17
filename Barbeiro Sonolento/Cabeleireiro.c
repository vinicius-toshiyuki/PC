#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define NV 5 /* numero de vagas */
#define NC 10

void * cliente(void *);
void * barbeiro(void *);

sem_t novoCliente, novaVaga;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(void){
	sem_init(&novoCliente, 0, 0);
	sem_init(&novaVaga, 0, NV);

	pthread_t clientes[NC], dorminhoco;

	for(int i = 0; i < NC; i++)
		pthread_create(&clientes[i], NULL, cliente, (void *) i);

	pthread_create(&dorminhoco, NULL, barbeiro, NULL);
	pthread_join(dorminhoco, NULL);

	sem_destroy(&novaVaga);
	sem_destroy(&novoCliente);

  return 0;
}

void * cliente(void *arg){
	int id = (int) arg;

	while(1){
		pthread_mutex_lock(&lock);
		pthread_mutex_unlock(&lock);
		sem_wait(&novaVaga);
		sem_post(&novoCliente);
		pthread_cond_signal(&cond);
		printf("%d: vou cortar\n", id);
		sleep(3);
	}

	pthread_exit(NULL);
}
void * barbeiro(void *arg){
	while(1){
		pthread_mutex_lock(&lock);
		sem_trywait(&novoCliente);
		if(EAGAIN == errno){
			errno = 0;
			printf("Indo dormir\n");
			pthread_cond_wait(&cond, &lock);
			sem_wait(&novoCliente);
		}
		pthread_mutex_unlock(&lock);
		sem_post(&novaVaga);
		printf("vou cortar cabelo\n");
	}

	pthread_exit(NULL);
}
