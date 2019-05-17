#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define C 5 /* Número de cadeiras */
#define NC 10 /* Número de clientes */
#define VAZIA 0
#define OCUPADA 1

int cadeiras[C] = {[0 ... C - 1] = VAZIA};
pthread_mutex_t temCadeiraLivre = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperaCortar = PTHREAD_COND_INITIALIZER;
sem_t clienteNaCadeira;

void * barbeiro(void *arg);
void * cliente(void *arg);

void cortaCabelo(void){ sleep(1); return; }
int checaDisponibilidade();

int main(void){
	sem_init(&clienteNaCadeira, 0, 0);

	pthread_t thds[NC + 1]; /* Um barbeiro */

	for(int i = 0; i < NC; i++){
		pthread_create(&thds[i], NULL, cliente, NULL);
	}
	pthread_create(&thds[NC], NULL, barbeiro, NULL);

	pthread_join(thds[0], NULL);

	sem_destroy(&clienteNaCadeira);

	return 0;
}

void * barbeiro(void *arg){
	while(1){
		sem_wait(&clienteNaCadeira);
		printf("Cortando\n");
		cortaCabelo();
		pthread_cond_signal(&esperaCortar);
	}
	pthread_exit(NULL);
}

void * cliente(void *arg){
	while(1){
		pthread_mutex_lock(&temCadeiraLivre);
		if(checaDisponibilidade()){
			pthread_mutex_unlock(&temCadeiraLivre);
			sem_post(&clienteNaCadeira);
			pthread_cond_wait(&esperaCortar, &temCadeiraLivre);
			printf("Agora estou careca\n");
		} else {
			pthread_mutex_unlock(&temCadeiraLivre);
			printf("Vou embora\n");
		}
	}
	pthread_exit(NULL);
}

int checaDisponibilidade(){
	int tem = 0;
	for(int i = 0; i < C; i++)
		if(cadeiras[i] == VAZIA) tem = 1;
	return tem;
}
