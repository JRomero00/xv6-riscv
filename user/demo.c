#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 10

int
main(int argc, char *argv[])
{
  int i, pid;
  
  printf("Demo Lottery Scheduling - Iniciando %d procesos\n", N);
  
  for(i = 0; i < N; i++) {
    pid = fork();
    
    if(pid < 0) {
      printf("Error en fork\n");
      exit(1);
    }
    
    if(pid == 0) {
      // Proceso hijo
      int tickets = 50 * (i + 1);
      settickets(tickets);
      
      printf("Proceso %d: tickets=%d\n", getpid(), tickets);
      
      // Realizar trabajo (ciclo intensivo de CPU)
      int j, k;
      for(j = 0; j < 100000; j++) {
        for(k = 0; k < 1000; k++) {
          // Trabajo simulado
        }
      }
      
      printf("Proceso %d finalizado\n", getpid());
      exit(0);
    }
  }
  
  // Padre espera a todos los hijos
  for(i = 0; i < N; i++) {
    wait(0);
  }
  
  printf("Demo completado\n");
  exit(0);
}
