#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t porteira = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t naoTemOQueFazer = PTHREAD_COND_INITIALIZER;

#define N 10
#define lock(porta) pthread_mutex_lock(porta)
#define unlock(porta) pthread_mutex_unlock(porta)
#define sleep(cond, lock) pthread_cond_wait(cond, lock);
#define wake(cond) pthread_cond_signal(cond);

int buffer[10] = {[0 ... 9] = 0};
int contador = 0;

void * produtor(void *arg);
void * consumidor(void *arg);

int main(void){
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
    while(contador == N - 1){
      sleep(&naoTemOQueFazer, &porteira);
    }
    unlock(&porteira);

     vitrine[contador] = rand() % 100;
     printf("Produzido %d, %d\n", vitrine[contador], contador);

    lock(&porteira);
    if(++contador == 1)
       wake(&naoTemOQueFazer);
    unlock(&porteira);  
  }

  pthread_exit(NULL);
}

void * consumidor(void *arg){
  int *vitrine = (int *) arg;

  while(1){
    lock(&porteira);
    while(!contador){
      sleep(&naoTemOQueFazer, &porteira);
    }
    unlock(&porteira);

    printf("Consumido %d, %d\n", vitrine[contador], contador);

    lock(&porteira);
    if(--contador == 0)
       wake(&naoTemOQueFazer);
    unlock(&porteira);  
  }

  pthread_exit(NULL);
}
