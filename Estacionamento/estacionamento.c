#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <sys/ioctl.h>

#define NP 3
#define NF 2
#define NA 5
#define NT NP + NF + NA
#define lock(mutex) pthread_mutex_lock(mutex)
#define unlock(mutex) pthread_mutex_unlock(mutex)
#define wait(cond, mutex) pthread_cond_wait(cond, mutex)
#define signal(cond) pthread_cond_signal(cond)
#define MUTEX PTHREAD_MUTEX_INITIALIZER
#define COND PTHREAD_COND_INITIALIZER
#define BG(cor) "\e[48;5;" #cor "m"
#define CLEARN "\e[0m\n"

int pnum = 0, fnum = 0, anum = 0, capacidade = 5, ocupadas = 0;
int pcor, fcor, acor;
int winheight;

void * professor(void *arg);
void * funcionario(void *arg);
void * aluno(void *arg);

pthread_cond_t pcond = COND, fcond = COND, acond = COND;
pthread_mutex_t mutex = MUTEX;

int main(int argc, char **argv){
	//printf("\e[s");
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	winheight = w.ws_row;
	printf("\e[2J");
	setlocale(LC_ALL, "");
	pthread_t thds[NT];
	int i;
	for(i = 0; i < NT; i++)
		pthread_create(&thds[i], NULL, i < NP ? professor : i < NP + NF ? funcionario : aluno, (void *) i);
	pthread_join(thds[0], NULL);	
	return 0;
}

void print(int *quem, int qual, int id){
	static int pcur[] = {0,0}, fcur[] = {0,35}, acur[] = {0,70};
	int *cur = quem == &pnum ? pcur : quem == &fnum ? fcur : acur;
	//printf("\e[u");
	if(pcur[0] == winheight || fcur[0] == winheight || acur[0] == winheight){
		pcur[0] = 0;
		fcur[0] = 0;
		acur[0] = 0;
		printf("\e[2J");
	}
	printf("\e[0;0H");
	printf("\e[%dB", cur[0]);
	printf("\e[%dC", cur[1]);
	fflush(stdout);
	switch(qual){
		case 1:
			printf("%s  \e[0m ", quem == &pnum ? BG(18) : quem == &fnum ? BG(125) : BG(44));
			printf("%ls %d esperando vaga" CLEARN, quem == &pnum ? L"Professor" : quem == &fnum ? L"Funcionário" : L"Aluno", id);
			break;
		case 2:
			printf("%s  \e[0m ", quem == &pnum ? BG(18) : quem == &fnum ? BG(125) : BG(44));
			printf("%ls %d indo estacionar" CLEARN, quem == &pnum ? L"Professor" : quem == &fnum ? L"Funcionário" : L"Aluno", id);
			break;
		case 3:
			printf("%s  \e[0m ", quem == &pnum ? BG(18) : quem == &fnum ? BG(125) : BG(44));
			printf("%ls %d estacionando" CLEARN, quem == &pnum ? L"Professor" : quem == &fnum ? L"Funcionário" : L"Aluno", id);
			break;
		case 4:
			printf("%s  \e[0m ", quem == &pnum ? BG(18) : quem == &fnum ? BG(125) : BG(44));
			printf("%ls %d saindo" CLEARN, quem == &pnum ? L"Professor" : quem == &fnum ? L"Funcionário" : L"Aluno", id);
			break;
		default:
			printf("Deu algo errado :[]\n");
			return;
	}
	cur[0]++;
	return;
}

void fun(pthread_cond_t *cond, int *quem, int tempo, int id){
	while(1){
		lock(&mutex); {
			while(ocupadas == capacidade){
				*quem++;
				print(quem, 1, id);
				wait(cond, &mutex);
				*quem--;
			}
			print(quem, 2, id);
			ocupadas++;
			print(quem, 3, id);
		} unlock(&mutex);
		sleep(tempo);
		lock(&mutex); {
			ocupadas--;
			print(quem, 4, id);
			if(pnum)
				signal(&pcond);
			else if(fnum)
				signal(&fcond);
			else if(anum)
				signal(&acond);
		} unlock(&mutex);
		sleep(tempo);
	}
	return;
}

void * professor(void *arg){
	fun(&pcond, &pnum, 5, (int) arg);
	pthread_exit(NULL);
}
void * funcionario(void *arg){
	fun(&fcond, &fnum, 2, (int) arg);
	pthread_exit(NULL);
}
void * aluno(void *arg){
	fun(&acond, &anum, 1, (int) arg);
	pthread_exit(NULL);
}
