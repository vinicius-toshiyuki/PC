#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define L 30
#define E 10

pthread_mutex_t portaDoLeitor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDoEscritor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDoFiscal = PTHREAD_MUTEX_INITIALIZER;

void * escritor(void *);
void * leitor(void *);

int main(void){
  pthread_t thdsl[L];
  pthread_t thdse[E];
  for(int i = 0; i < L; i++)
    pthread_create(&thdsl[i], NULL, leitor, (void *) i);
  for(int i = 0; i < E; i++)
    pthread_create(&thdse[i], NULL, escritor, NULL);
  for(int i = 0; i < L; i++)
    pthread_join(thdsl[i], NULL);
  for(int i = 0; i < E; i++)
    pthread_join(thdse[i], NULL);
  return 0;
}
void * escritor(void *arg1){
  while(1){
    pthread_mutex_lock(&portaDoFiscal);
    pthread_mutex_lock(&portaDoEscritor);
    pthread_mutex_unlock(&portaDoFiscal);

    printf("Escritor escrevendo...\n");
    sleep(3);

    pthread_mutex_unlock(&portaDoEscritor);
  }
  pthread_exit(NULL);
}
void * leitor(void *arg1){
  while(1){
    int id = (int) arg1;
    static int leitores = 0;
    pthread_mutex_lock(&portaDoFiscal);
    pthread_mutex_lock(&portaDoLeitor);
    if(!leitores){
      pthread_mutex_lock(&portaDoEscritor);
    }
    leitores++;
    pthread_mutex_unlock(&portaDoLeitor);
    pthread_mutex_unlock(&portaDoFiscal);

    printf("Leitor %d lendo...\n", id);
    sleep(1);

    pthread_mutex_lock(&portaDoLeitor);
    if(leitores == 1){
      pthread_mutex_unlock(&portaDoEscritor);
    }
    leitores--;
    pthread_mutex_unlock(&portaDoLeitor);
  }

  pthread_exit(NULL);
}
