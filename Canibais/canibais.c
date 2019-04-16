#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>

#define NC 10
#define PORCOES 5
#define Bg(cor) strcat(pat, itoa(cor))

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t conz = PTHREAD_COND_INITIALIZER;

void * cozinheiro(void *arg);
void * canibal(void *arg);

int porcao = 1;
int qtd = 0;
int qtotal = 0;

int main(void){
  int mesa[PORCOES] = {[0 ... PORCOES - 1] = 0};

	pthread_t thds[NC + 1];
  pthread_create(thds, NULL, cozinheiro, (void *) mesa);
  for(int i = 1; i < NC; i++){
    pthread_create(&thds[i], NULL, canibal, (void *) mesa);
  }
  pthread_join(thds[0], NULL);
  return 0;
}

void * cozinheiro(void *arg){
  int *mesa = (int *) arg;
  while(1){
    pthread_mutex_lock(&lock);
    if(qtd != 0){
      printf("[\e[48;5;227m  \e[0m] \e[38;5;229mAi, que cansado.. Vou dormir. Enchi a mesa.\e[0m\n");
      pthread_cond_wait(&conz, &lock);
    }
    while(qtd < PORCOES){
      printf("[\e[48;5;227m  \e[0m] \e[38;5;221mMama mia! Fazendo sopa %d!\e[0m\n", ++qtotal);
      mesa[qtd++] = qtotal;
    }
    sleep(2);

    printf("[\e[48;5;227m  \e[0m] \e[38;5;215mDing-dong! Hora de acordar e comer!\n");
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&lock);
  }
  pthread_exit(NULL);
}

void * canibal(void *arg){
  int *mesa = (int *) arg;
  int minha;
  while(1){
    pthread_mutex_lock(&lock);
    while(qtd == 0){
      printf("[\e[48;5;167m  \e[0m] \e[38;5;173m>:c %ls comida. Cozinha! Mesa vazia!\e[0m\n", L"Cadê");
      pthread_cond_signal(&conz);
      printf("[\e[48;5;167m  \e[0m] \e[38;5;174mIr dormir. Acordar quando pronto.\e[0m\n");
      pthread_cond_wait(&cond, &lock);
    }
    minha = mesa[--qtd];
    mesa[qtd] = 0;
    pthread_mutex_unlock(&lock);
    printf("[\e[48;5;167m  \e[0m] \e[38;5;168mIr comer comida %d hehe >:d\e[0m\n", minha);
  }

  pthread_exit(NULL);
}
