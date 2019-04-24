#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#define MINUTO 60
#define CURTO (rand() % 3) + 3
#define LONGO (rand() % 16) + 15

void ciclo_pomodoro(void);
void descansa(int tempo);
void trabalha(int tempo);
void terminal(int sig);

int main(void){
  int pid = fork();
  if(!pid){
    signal(1, terminal);
    srand(time(NULL));
    printf("Começando a ciclar\n");
    ciclo_pomodoro();
  }else{
    getchar();
    kill(pid, 1);
  }
  return 0;
}

void ciclo_pomodoro(){
  while(1){
    for(int i = 0; i < 3; i++){
      trabalha(25);
      descansa(CURTO);
    }
    trabalha(25);
    descansa(LONGO);
  }
  return;
}

void descansa(int tempo){
  sleep(tempo * MINUTO);
  printf("Passaram %d minuto%s. Volta a trabalhar!\n", tempo, tempo > 1 ? "s" : "");
  return;
}
void trabalha(int tempo){
  sleep(tempo * MINUTO);
  printf("Passaram %d minuto%s. Descansa!\n", tempo, tempo > 1 ? "s" : " ");
  return;
}
void terminal(int sig){
  printf("Encerrando\n");
  exit(-1);
}
