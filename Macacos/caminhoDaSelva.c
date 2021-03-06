#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#define L 30
#define G 2
#define lock(mutex) pthread_mutex_lock(mutex)
#define unlock(mutex) pthread_mutex_unlock(mutex)

//pthread_mutex_t portaDoBando[] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t portaDoBandoUm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDoBandoDois = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDoGorila = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDoFiscal = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDaPassarela = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t portaDoQuerer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t espera = PTHREAD_COND_INITIALIZER;

void * caminhoDaSelva(void *);
void imprimeMacaco(int direcao);
int gorilaQuerPassar(int mode);

int main(void){
  setlocale(LC_ALL, "");
  pthread_t thds[L + G];
  for(int i = 0; i < L; i++){
    int *info = (int *) malloc(sizeof(int) * 2);
    info[0] = i % 2 + 1;
    info[1] = i % 2 == 0 ? i / 2 + 1 : (i - 1) / 2 + 1;
    pthread_create(&thds[i], NULL, caminhoDaSelva, (void *) info);
  }
  for(int i = L; i < L + G; i++){
    int *info = (int *) malloc(sizeof(int) * 2);
    info[0] = i % 2 + 3;
    info[1] = i % 2 == 0 ? i / 2 + 1 : (i - 1) / 2 + 1;
    pthread_create(&thds[i], NULL, caminhoDaSelva, (void *) info);
  }
  for(int i = 0; i < L; i++)
    pthread_join(thds[i], NULL);
  return 0;
}
void * caminhoDaSelva(void *arg1){
  static int macacos1 = 0;
  static int macacos2 = 0;
  static int gorilas1 = 0;
  static int gorilas2 = 0;
  int bando = ((int *) arg1)[0];
  while(1){
    int id = ((int *) arg1)[1];
    if(bando == 1){
      lock(&portaDoFiscal);
      while(gorilaQuerPassar(0)){
        pthread_cond_wait(&espera, &portaDoFiscal);
      }
      lock(&portaDoBandoUm);
      if(macacos1++ == 0)
        lock(&portaDoBandoDois);
      unlock(&portaDoBandoUm);
      unlock(&portaDoFiscal);

      printf("\e[7;35mMacaco %d do bando %d passando. Uh-Uhh!\n", id, bando);
      //imprimeMacaco(bando);
      printf("\e[0m\n");
      sleep(1);

      lock(&portaDoBandoUm);
      if(macacos1-- == 1)
        unlock(&portaDoBandoDois);
      unlock(&portaDoBandoUm);
    } else if(bando == 2){
      lock(&portaDoFiscal);
      while(gorilaQuerPassar(0)){
        pthread_cond_wait(&espera, &portaDoFiscal);
      }
      lock(&portaDoBandoDois);
      if(macacos2++ == 0)
        lock(&portaDoBandoUm);
      unlock(&portaDoBandoDois);
      unlock(&portaDoFiscal);

      printf("\e[7;36mMacaco %d do bando %d passando. Uh-Uhh!\n", id, bando);
      //imprimeMacaco(bando);
      printf("\e[0m\n");
      sleep(1);

      lock(&portaDoBandoDois);
      if(macacos2-- == 1)
        unlock(&portaDoBandoUm);
      unlock(&portaDoBandoDois);
    } else if(bando == 3){
      sleep(1);
      gorilaQuerPassar(1);
      lock(&portaDoFiscal);
      lock(&portaDoBandoUm);
      lock(&portaDoGorila);
      if(gorilas1++ == 0){
        lock(&portaDoBandoDois);
      }
      unlock(&portaDoGorila);
      unlock(&portaDoBandoUm);
      unlock(&portaDoFiscal);

      printf("\e[7;36mGorila %d do bando %d passando. Uh-Uhh!\n", id, bando);
      //imprimeMacaco(bando);
      printf("\e[0m\n");
      sleep(1);

      lock(&portaDoBandoUm);
      lock(&portaDoGorila);
      if(gorilas1-- == 1){
        unlock(&portaDoBandoDois);
      }
      unlock(&portaDoGorila);
      unlock(&portaDoBandoUm);
      gorilaQuerPassar(-1);


    } else if(bando == 4){
      sleep(1);
      gorilaQuerPassar(1);
      lock(&portaDoFiscal);
      lock(&portaDoBandoDois);
      lock(&portaDoGorila);
      if(gorilas2++ == 0){
        lock(&portaDoBandoUm);
      }
      unlock(&portaDoGorila);
      unlock(&portaDoBandoDois);
      unlock(&portaDoFiscal);

      printf("\e[7;36mGorila %d do bando %d passando. Uh-Uhh!\n", id, bando);
      //imprimeMacaco(bando);
      printf("\e[0m\n");
      sleep(1);

      lock(&portaDoBandoDois);
      lock(&portaDoGorila);
      if(gorilas2-- == 1){
        unlock(&portaDoBandoUm);
      }
      unlock(&portaDoGorila);
      unlock(&portaDoBandoDois);
      gorilaQuerPassar(-1);
    }
  }

  pthread_exit(NULL);
}
void imprimeMacaco(int direcao){
  static int temalguem;
  lock(&portaDaPassarela);
  int apaga = 0;
  for(int i = 0; i < 4; i++){
    if(direcao == 1){
      printf("%s%ls", i == 0 ? "" : "\b ", L"🐵");
			fflush(stdout);
			sleep(1);
    }else{
			if(i == 0) printf("       \e[7D"), fflush(stdout);
			printf("\e[%dC", 7 - ++apaga);
      printf("%s%ls", i == 0 ? "" : "\b\b\b", L"🐵");
			fflush(stdout);
			sleep(1);
			printf("\b ");
			printf("\e[%dD", 7 - apaga);
			fflush(stdout);
    }
 }
 if(direcao) printf("\b "), fflush(stdout);
}

int gorilaQuerPassar(int mode){
  static int querem = 0;
  int ret = 0;
  lock(&portaDoQuerer);
  if(mode == 0) ret = querem;
  else{
    querem += mode;
    if(querem <= 0){
      pthread_cond_broadcast(&espera);
    }
  }
  unlock(&portaDoQuerer);
  return ret;
}