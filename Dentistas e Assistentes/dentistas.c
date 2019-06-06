#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define I 35
int linhas, colunas, enable = 1, last = 1;
unsigned short toggle = 0x000, simple = 0, nowait = 0;
#define printi(off, printargs...) { \
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); \
	colunas = w.ws_col; \
	enable = colunas < (I + 1) * 3 ? 0 : 1; \
	if(last != enable){ \
		printf("\e[2J\e[0;0H"); \
		last = enable; \
		toggle = 0x111; \
	} \
	if(enable && !simple) \
{ \
	lock(&mutex); \
	if(off[0] == linhas - 1 || off[0] == 1 || toggle){ \
		toggle &= !off[1] ? 0x011 : off[1] == I ? 0x010 : 0x001; \
		off[0] = 1; \
		int i; \
		char spaco[I + 1] = {[0 ... I - 1] = ' '}; \
		spaco[I] = '\0'; \
		for(i = 0; i < linhas; i++){ \
			printf("\e[48;5;%dm\e[%d;%dH%s\e[0m", off[2], i, off[1] + 1, spaco); \
		} \
	} \
	printf("\e[48;5;%dm\e[38;5;%dm\e[0;0H\e[%dB\e[%dC", off[2], off[3], off[0]++, off[1] + 1); \
	fflush(stdout); \
	printf(printargs); \
	printf("\e[0m"); \
	unlock(&mutex); \
}else \
	printf(printargs); \
}

#define DENTISTA 10
#define ASSISTENTE 20
#define PACIENTE 40
#define PESSOAS DENTISTA + ASSISTENTE + PACIENTE

#define lock(mutex) pthread_mutex_lock(mutex)
#define unlock(mutex) pthread_mutex_unlock(mutex)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * dentista(void *arg);
void * assistente(void *arg);
void * paciente(void *arg);
void * espera(void *arg);

sem_t spacientes, sdentista, ajuda, sassistentes;
struct winsize w;

int c(const void *a, const void *b){
	return strcmp(*(char **)a, *(char **)b);
}

int main(int argc, char **argv){
	if(argc > 1){
		char *args[argc];
		memcpy(args, argv, sizeof(char *) * argc);
		qsort(argv, argc, sizeof(char *), (__compar_fn_t) c);
		for(int i = 0; i < argc; i++) printf("%s\n", argv[i]);
		char *flags[] = {"-s", "-w"};
		simple =  bsearch(flags, argv, argc, sizeof(char *), (__compar_fn_t) c) ? 1 : 0;
		nowait =  bsearch(flags + 1, argv, argc, sizeof(char *), (__compar_fn_t) c) ? 1 : 0;
	}
	srand(time(NULL));
	if(!simple){
		printf("\e[?25l");
		printf("\e[2J");
	}
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); \
	colunas = w.ws_col; \
	linhas = w.ws_row;

	pthread_t thds[PACIENTE], finalizador;
	sem_init(&spacientes, 0, PACIENTE);
	sem_init(&sdentista, 0, DENTISTA);
	sem_init(&sassistentes, 0, ASSISTENTE);

	int i;
	for(i = 0; i < PACIENTE; i++){
		int *id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&thds[i], NULL, paciente, (void *) id);
	}
	pthread_create(&finalizador, NULL, espera, NULL);
	pthread_join(finalizador, NULL);
	if(!simple){
		printf("\e[?25h");
		printf("\e[2J\e[0;0H");
	}
	return 0;
}

void * paciente(void *arg){
	static int off[] = {1,0,27,234};
	int id = *(int *) arg;
	while(1){
		sem_wait(&spacientes);
		printi(off, "[%2d] Oi, sou paciente %d\n", id, id);
		printi(off, "[%2d]  Quero ser atendido\n", id);
		sem_wait(&sdentista);
		pthread_t doto;
		pthread_create(&doto, NULL, dentista, arg);
		pthread_join(doto, NULL);
		printi(off, "[%2d]  Tchau\n", id);
		sem_post(&spacientes);
	}
	pthread_exit(NULL);
}

void * dentista(void *arg){
	static int off[] = {1,I,54,45};
	int id = *(int *) arg;

	printi(off, "[%2d] Oi, sou dentista %d\n", id, id);
	printi(off, "[%2d]   Vou atender cliente %d\n", id, id);
	if(rand() % 2){
		int qt = 1, i, res, par;
		sem_getvalue(&sassistentes, &qt);
		qt = rand() % (qt > 0 ? qt : 1) + 1;
		printi(off, "[%2d]   Quero %d ajuda\n", id, qt);
		pthread_t assis[qt];
		for(i = 0; i < qt; i++){
			if(sem_trywait(&sassistentes)){
				pthread_create(&assis[i], NULL, assistente, NULL);
				continue;
			}
			void *value = malloc(sizeof(int) * 2);
			((int *) value)[0] = id;
			((int *) value)[1] = i;
			pthread_create(&assis[i], NULL, assistente, value);
		}
		for(i = 0; i < qt; i++)
			pthread_join(assis[i], NULL);
	}else{
		printi(off, "[%2d]   Ha. Nem preciso de ajuda\n", id);
	}
	if(!nowait) sleep(3);
	printi(off, "[%2d]   Terminei de atender\n", id);
	sem_post(&sdentista);
	pthread_exit(NULL);
}

void * assistente(void *arg){
	static int off[] = {1,I<<1,78,9};
	if(arg){
					int id = *((int *) arg), eu = *((int *) arg + 1);
					printi(off, "[%2d] Oi, sou assistente %d\n", eu, eu);
					printi(off, "[%2d]   Vou ajudar %d\n", eu, id);
					if(!nowait) sleep(rand() % 4);
	}
	sem_post(&sassistentes);
	if(arg) free(arg);
	pthread_exit(NULL);
}

void * espera(void *arg){
	if(!nowait) getchar();
	else for(int i = 0; i < 100; i++);
	pthread_exit(NULL);
}
