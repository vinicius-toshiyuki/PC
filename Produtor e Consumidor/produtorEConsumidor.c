#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

pthread_mutex_t porteira = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t semEspaco = PTHREAD_COND_INITIALIZER;
pthread_cond_t semProdutos = PTHREAD_COND_INITIALIZER;

#define N 10
#define lock(porta) pthread_mutex_lock(porta)
#define unlock(porta) pthread_mutex_unlock(porta)
#define wait(cond, lock) pthread_cond_wait(cond, lock);
#define signal(cond) pthread_cond_signal(cond);

int buffer[10] = {[0 ... 9] = 0};
int contador = 0;

void * produtor(void *arg);
void * consumidor(void *arg);

int main(void){
  setlocale(LC_ALL, "");
  srand(time(NULL));
  pthread_t thds[N];
  for(int i = 0; i < N; i++){
      pthread_create(&thds[i], NULL, i % 2 == 0 ? produtor : consumidor, buffer);
  }
  pthread_join(thds[0], NULL);
	return 0;
}

void * produtor(void *arg){
  int *vitrine = (int *) arg;

  while(1){
    lock(&porteira);
    while(contador == N){
      printf("[\e[48;5;33m  \e[0m] \e[38;5;27mProdutor indo dormir, vitrine cheia.\e[0m\n");
      wait(&semEspaco, &porteira);
    }
    vitrine[contador] = rand() % 100;
    printf("[\e[48;5;33m  \e[0m] \e[38;5;45mProduzido %d, colocado na %ls %d da vitrine\e[0m\n", vitrine[contador], L"posição", contador);
    contador++;
    unlock(&porteira);

    //sleep(3);

    lock(&porteira);
    if(contador == 1)
       printf("[\e[48;5;33m  \e[0m] \e[38;5;87mAcordando consumidor, vitrine %ls vazia\e[0m\n", L"não");
       signal(&semProdutos);
    unlock(&porteira);  
  }

  pthread_exit(NULL);
}

void * consumidor(void *arg){
  int *vitrine = (int *) arg;

  while(1){
    lock(&porteira);
    while(contador == 0){
      printf("[\e[48;5;34m  \e[0m] \e[38;5;28mConsumidor indo dormir, vitrine vazia\e[0m\n");
      wait(&semProdutos, &porteira);
    }
    contador--;
    printf("[\e[48;5;34m  \e[0m] \e[38;5;46mConsumido %d, pego da %ls %d da vitrine\e[0m\n", vitrine[contador], L"posição", contador);
    unlock(&porteira);
 
    //sleep(3);

    lock(&porteira);
    if(contador == N - 1)
       printf("[\e[48;5;34m  \e[0m] \e[38;5;70mAcordando produtor, %ls liberado.\e[0m\n", L"espaço");
       signal(&semEspaco);
    unlock(&porteira);  
  }

  pthread_exit(NULL);
}
