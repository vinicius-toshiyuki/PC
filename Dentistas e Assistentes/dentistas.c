#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define DENTISTA 1
#define ASSISTENTE 2
#define PACIENTE 4
#define PESSOAS DENTISTA + ASSISTENTE + PACIENTE

void * dentista(void *arg);
void * assistente(void *arg);
void * paciente(void *arg);

sem_t spacientes, sdentista, sassistentes, ajuda;

int linhas, colunas;

int main(int argc, char **argv){
	srand(time(NULL));

	pthread_t thds[PACIENTE];

	// sem_init(&ajuda, 0, 0);
	sem_init(&spacientes, 0, 0);
	sem_init(&sdentista, 0, DENTISTA);
	sem_init(&sassistentes, 0, ASSISTENTE);

	int i;
	for(i = 0; i < PACIENTE; i++)
		pthread_create(&thds[i], NULL, paciente, (void *) i);
	for(i = 0; i < PACIENTE; i++)
		pthread_join(thds[i], NULL);
	return 0;
}

void * paciente(void *arg){
	int id = (int) arg;
	while(1){
		printf("Oi, sou paciente %d\n", id); 
		// Paciente chega
		// Espera um DENTISTA
		printf("\tQuero ser atendido\n"); 
		sem_wait(&sdentista);
		pthread_t doto;
		pthread_create(&doto, NULL, dentista, (void *) id);
		// É atendido
		pthread_join(doto, NULL);
		printf("\tTchau\n"); 
	}
	pthread_exit(NULL);
}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void * dentista(void *arg){
	int id = (int) arg;

	printf("Oi, sou dentista %d\n", id); 
	printf("Vou atender cliente %d\n", id); 
	// Vê se precisa de ajuda
	if(rand() % 2){ // Avisa que precisa de ajuda
		printf("Quero ajuda\n"); 
		int qt, i, res, par;
		pthread_mutex_lock(&lock);
		sem_getvalue(&sassistentes, &qt);
		pthread_mutex_unlock(&lock);
		qt = rand() % qt;
		pthread_t assis[qt];
		for(i = 0; i < qt; i++){
			sem_wait(&sassistentes);
			void *value = malloc(sizeof(int) * 2);
			((int *) value)[0] = id;
			((int *) value)[1] = i;
			pthread_create(&assis[i], NULL, assistente, value);
		}
		for(i = 0; i < qt; i++)
			pthread_join(assis[i], NULL);
	}else{
		printf("Ha. Nem preciso de ajuda\n"); 
	}
	sleep(3);
	printf("Terminei de atender\n");
	// Libera mais um dentista
	sem_post(&sdentista);
	// Reseta para não estar precisando de ajuda
	pthread_exit(NULL);
}

void * assistente(void *arg){
	static int off[] = {0,60};
	int id = *((int *) arg), eu = *((int *) arg + 1);
	printf("Oi, sou assistente %d\n", eu); 
	fflush(stdout);
	// Espera poder ajudar
	printf("Vou ajudar %d\n", id); 
	sem_post(&sassistentes);
	sleep(rand() % 4);
	pthread_exit(NULL);
}
