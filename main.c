#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // fork()
#include <sys/wait.h> // define wait
#include <string.h>

#define MAX_LENGTH 1024 // tamanho maximo do comando

int main() {
  char command[MAX_LENGTH];

  // loop principal
  while (1) {
    printf("  >> ");
    fgets(command, MAX_LENGTH, stdin);

    // remove a quebra de linha no final do comando
    command[strcspn(command, "\n")] = 0;

    // cria um novo processo filho para executar o comando
    pid_t pid = fork();

    if (pid == 0) {
      // processo filho - executa o comando
      execlp(command, command, NULL);
    } else {
      // processo pai - espera pelo filho terminar
      wait(NULL);
    }
  }

  return 0;
}
