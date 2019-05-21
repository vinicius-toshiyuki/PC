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
#define RESET printf("\e[0m");
#define RESETN printf("\e[0m\n");

int dentistas_ajuda[DENTISTA];
int ajudando[DENTISTA];

typedef void * (*P_FUN)(void *arg);

int cor(int cod){printf("\e[48;5;%dm", cod); return 0;}

void * dentista(void *arg);
void * assistente(void *arg);
void * paciente(void *arg);

sem_t spacientes, sdentista, ajuda, sassistentes;

int linhas, colunas;

int main(int argc, char **argv){
	srand(time(NULL));

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	linhas = w.ws_row;
	colunas = w.ws_col;

	pthread_t thds[PACIENTE];

	sem_init(&spacientes, 0, 0);
	sem_init(&sdentista, 0, DENTISTA);
	sem_init(&ajuda, 0, 0);
	sem_init(&sassistentes, 0, ASSISTENTE);

	printf("Teste\e[2J", cor(0)), RESETN;

	int i;
	for(i = 0; i < PACIENTE; i++)
		pthread_create(&thds[i], NULL, paciente, (void *) i);
	for(i = 0; i < PACIENTE; i++)
		pthread_join(thds[i], NULL);
	return 0;
}

#define lock(mutex) pthread_mutex_lock(mutex)
#define unlock(mutex) pthread_mutex_unlock(mutex)

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int set(int *off){ lock(&mutex); printf("\e[0;0H\e[%dB\e[%dC", off[0]++, off[1]); fflush(stdout); unlock(&mutex); return 0; }
void * dentista(void *arg){
	static int off[] = {0,30};
	int id = (int) arg;

	printf("Oi, sou dentista %d\n", id, set(off));
	printf("Vou atender cliente %d\n", id, set(off));
	// Vê se precisa de ajuda
	if(rand() % 2){
		// Avisa que precisa de ajuda
		printf("Quero ajuda\n", set(off));
		int qt, i, res, par;
		sem_getvalue(&sassistentes, &qt);
		qt = rand() % qt;
		pthread_t assis[qt];
		for(i = 0; i < qt; i++){
			void *value = malloc(sizeof(int) * 2);
			((int *) value)[0] = id;
			((int *) value)[1] = i;
			pthread_create(&assis[i], NULL, assistente, value);
		}
		for(i = 0; i < qt; i++)
			pthread_join(assis[i], NULL);
	}else{
		printf("Ha. Nem preciso de ajuda\n", set(off));
	}
	sleep(3);
	printf("Terminei de atender\n", set(off));
	// Libera mais um dentista
	sem_post(&sdentista);
	// Reseta para não estar precisando de ajuda
	pthread_exit(NULL);
}

void * assistente(void *arg){
	static int off[] = {0,60};
	int id = *((int *) arg), eu = *((int *) arg + 1);
	printf("Oi, sou assistente %d\n", eu, set(off));
	// Espera poder ajudar
	printf("Vou ajudar %d\n", id, set(off));
	sleep(rand() % 4);
	pthread_exit(NULL);
}

void * paciente(void *arg){
	static int off[] = {0,0};
	int id = (int) arg;
	printf("Oi, sou paciente %d\n", id, set(off));
	while(1){
		// Paciente chega
		// Espera um DENTISTA
		printf("Quero ser atendido\n", set(off));
		sem_wait(&sdentista);
		pthread_t doto;
		pthread_create(&doto, NULL, dentista, (void *) id);
		// É atendido
		pthread_join(doto, NULL);
		printf("Tchau\n", set(off));
	}
	pthread_exit(NULL);
}
