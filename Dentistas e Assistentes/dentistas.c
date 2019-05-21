#include <stdio.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <semaphore.h>

#define DENTISTA 5
#define ASSISTENTE 10
#define PACIENTE 20
#define PESSOAS DENTISTA + ASSISTENTE + PACIENTE
#define RESET printf("\e[0m");

typedef void * (*P_FUN)(void *arg);

int cor(int cod){printf("\e[48;5;%dm", cod); return 0;}

void * dentista(void *arg);
void * assistente(void *arg);
void * paciente(void *arg);

sem_t spacientes;

int main(int argc, char **argv){
	pthread_t thds[DENTISTA + ASSISTENTE + PACIENTE];

	printf("Teste", cor(132)), RESET;

	P_FUN f = dentista;

	int i;
	for(i = 0; i < PESSOAS; i++)
		pthread_create(&thds[i], NULL, f, NULL), f = i < DENTISTA ? dentista : i < DENTISTA + ASSISTENTE ? assistente : paciente;
	for(i = 0; i < PESSOAS; i++)
		pthread_join(thds[i], NULL);
	return 0;
}

void * dentista(void *arg){
	printf("Oi, sou dentista\n");
	while(1){
		sem_wait(&spacientes);
		printf("Vou atender\n");
		if(rand() % 2){
			printf("Quero ajuda\n");
		}else{
			printf("Ha. Nem preciso de ajuda\n");
		}
		printf("Terminei de atender\n");
		sem_post(&sdentista);
	}
	pthread_exit(NULL);
}

void * assistente(void *arg){
	printf("Oi, sou assistente\n");
	while(1){
		sem_trywait(&dentista);
		printf("Vou ajudar\n")
	}
	pthread_exit(NULL);
}

void * paciente(void *arg){
	printf("Oi, sou paciente\n");
	pthread_exit(NULL);
}
