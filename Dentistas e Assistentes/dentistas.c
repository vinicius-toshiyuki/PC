#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

int linhas, colunas;
#define printi(off, printargs...) \
{ \
	lock(&mutex); \
	if(off[0] == linhas - 1){ \
		off[0] = 1; \
		int i; \
		char spaco[30] = {[0 ... 29] = ' '}; \
		for(i = 0; i < linhas; i++){ \
			printf("\e[%d;%dH%s", i, off[1], spaco); \
		} \
	} \
	printf("\e[0;0H\e[%dB\e[%dC", off[0]++, off[1]); \
	fflush(stdout); \
	printf(printargs); \
	unlock(&mutex); \
}

#define DENTISTA 1
#define ASSISTENTE 2
#define PACIENTE 4
#define PESSOAS DENTISTA + ASSISTENTE + PACIENTE

#define lock(mutex) pthread_mutex_lock(mutex)
#define unlock(mutex) pthread_mutex_unlock(mutex)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * dentista(void *arg);
void * assistente(void *arg);
void * paciente(void *arg);

sem_t spacientes, sdentista, ajuda, sassistentes;

int main(int argc, char **argv){
	srand(time(NULL));
	printf("\e[2J");
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	linhas = w.ws_row;
	colunas = w.ws_col;

	pthread_t thds[PACIENTE];
	sem_init(&spacientes, 0, 0);
	sem_init(&sdentista, 0, DENTISTA);
	sem_init(&sassistentes, 0, ASSISTENTE);

	int i;
	for(i = 0; i < PACIENTE; i++){
		int *id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&thds[i], NULL, paciente, (void *) id);
	}
	for(i = 0; i < PACIENTE; i++)
		pthread_join(thds[i], NULL);
	return 0;
}

void * paciente(void *arg){
	static int off[] = {1,0};
	int id = *(int *) arg;
	while(1){
		printi(off, "Oi, sou paciente %d\n", id);
		printi(off, "\tQuero ser atendido\n");
		sem_wait(&sdentista);
		pthread_t doto;
		pthread_create(&doto, NULL, dentista, arg);
		pthread_join(doto, NULL);
		printi(off, "\tTchau\n");
	}
	pthread_exit(NULL);
}

void * dentista(void *arg){
	static int off[] = {1,30};
	int id = *(int *) arg;

	printi(off, "Oi, sou dentista %d\n", id);
	printi(off, "\tVou atender cliente %d\n", id);
	if(rand() % 2){
		int qt = 1, i, res, par;
		sem_getvalue(&sassistentes, &qt);
		qt = rand() % qt + 1;
		printi(off, "\tQuero %d ajuda\n", qt);
		pthread_t assis[qt];
		for(i = 0; i < qt; i++){
			sem_trywait(&sassistentes);
			void *value = malloc(sizeof(int) * 2);
			((int *) value)[0] = id;
			((int *) value)[1] = i;
			pthread_create(&assis[i], NULL, assistente, value);
		}
		for(i = 0; i < qt; i++)
			pthread_join(assis[i], NULL);
	}else{
		printi(off, "\tHa. Nem preciso de ajuda\n");
	}
	sleep(3);
	printi(off, "\tTerminei de atender\n");
	sem_post(&sdentista);
	pthread_exit(NULL);
}

void * assistente(void *arg){
	static int off[] = {1,60};
	int id = *((int *) arg), eu = *((int *) arg + 1);
	printi(off, "Oi, sou assistente %d\n", eu);
	printi(off, "\tVou ajudar %d\n", id);
	sem_post(&sassistentes);
	sleep(rand() % 4);
	free(arg);
	pthread_exit(NULL);
}
