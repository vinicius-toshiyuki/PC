#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#define lock(mutex)pthread_mutex_lock(mutex)
#define unlock(mutex)pthread_mutex_unlock(mutex)
#define atomic_soma(s,v)\
lock(&mutex);{\
s+=v;\
}unlock(&mutex);
#define QT 5
#define prog pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;pthread_barrier_t barrera;int soma=0,arr[QT];void*thread(void*arg);int main(int argc,char **argv){srand(time(NULL));pthread_t thds[QT];pthread_barrier_init(&barrera,NULL,QT);int i;for(i=0;i<QT;i++){int *id=(int*)malloc(sizeof(int));*id=i+1;pthread_create(&thds[i],NULL,thread,(void*)id);}pthread_join(thds[0],NULL);pthread_barrier_destroy(&barrera);return 0;}void*thread(void*arg){atomic_soma(soma,arr[*(int*)arg-1]=rand()%255);printf("Olha o meu numero: %d\n",arr[*(int*)arg-1]);pthread_barrier_wait(&barrera);printf("Olha aí o resultado: %d\n",soma);free(arg);pthread_exit(NULL);}
prog
