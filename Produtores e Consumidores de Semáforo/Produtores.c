#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define NP 10
#define NC 10
#define BUFFER_SIZE 10

sem_t vazio;
sem_t ocupado;
pthread_mutex_t lock_produtor = PTHREAD_MUTEX_INITIALIZER;

void * produtores(void *arg);
void * consumidores(void *arg);

int main(void){
  srand(time(NULL));
  int buffer[BUFFER_SIZE] = {[0 ... BUFFER_SIZE - 1] = -1};

  sem_init(&vazio, 0, BUFFER_SIZE);
  sem_init(&ocupado, 0, 0);

  pthread_t thds[NP + NC];
  
  int index = -1;
  for(int i = 0; i < NP + NC; i++){
    void *arg = malloc(sizeof(int *) + 2 * sizeof(int *));
    int *argi = (int *) arg;
    int **argp = (int **) (arg + sizeof(int)); 
    int **argindex = (int **) (arg + sizeof(int) + sizeof(int *));
    *argi = i;
    *argp = buffer;
    *argindex = &index;
    pthread_create(&thds[i], NULL, i < NP ? produtores : consumidores, arg);
  }

  pthread_join(thds[0], NULL);

  sem_destroy(&vazio);
  sem_destroy(&ocupado);

  return 0;
}

void * produtores(void *arg){
  int id = *((int *) arg);
  int *buffer = *((int **) (arg + sizeof(int)));
  int *index = *((int **) (arg + sizeof(int) + sizeof(int *)));

  while(1){
    sem_wait(&vazio);
    pthread_mutex_lock(&lock_produtor);

    buffer[++*index] = rand() % 100 + 1;
    sleep(1); // Produz
    printf("Produtor %d produziu %d\n", id, buffer[*index]);

    pthread_mutex_unlock(&lock_produtor);    
    sem_post(&ocupado);
  }

  pthread_exit(NULL);
}

void * consumidores(void *arg){
  int id = *((int *) arg);
  int *buffer = *((int **) (arg + sizeof(int)));
  int *index = *((int **) (arg + sizeof(int) + sizeof(int *)));

  while(1){
    sem_wait(&ocupado);
    pthread_mutex_lock(&lock_produtor);

    int item = buffer[*index]; buffer[(*index)--] = -1;
    printf("Consumidor %d consumiu %d\n", id - NP, item);

    pthread_mutex_unlock(&lock_produtor);
    sem_post(&vazio);
  }

  pthread_exit(NULL);
}
