#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h> // define wait
#include <unistd.h>   // fork()

#define MAX_LENGTH 1024 // tamanho máximo do comando

int main() {
  char command[MAX_LENGTH];

  while (1) {
    printf("$ ");
    fgets(command, MAX_LENGTH, stdin);

    // remove a quebra de linha no final do comando
    command[strcspn(command, "\n")] = 0;

    // verifica se há modificadores
    char *args[MAX_LENGTH];
    int arg_count = 0;
    char *token = strtok(command, " ");
    char *input_file = NULL;
    char *output_file = NULL;
    char *pipe_command = NULL;
    int background = 0;

    while (token != NULL) {
      if (strcmp(token, "<") == 0) {
        // modificador de ENTRADA de arquivo
        token = strtok(NULL, " ");
        input_file = token;
      } else if (strcmp(token, ">") == 0) {
        // modificador de SAIDA de arquivo
        token = strtok(NULL, " ");
        output_file = token;
      } else if (strcmp(token, "|") == 0) {
        // modificador pipe
        token = strtok(NULL, " ");
        pipe_command = token;
      } else if (strcmp(token, "&") == 0) {
        // execução em segundo plano
        background = 1;
        break;
      } else {
        args[arg_count] = token;
        arg_count++;
      }

      token = strtok(NULL, " ");
    }

    if (arg_count > 0) {
      args[arg_count] = NULL;

      // cria um novo processo filho para executar o comando
      pid_t pid = fork();

      if (pid == 0) {
        // processo filho - executa o comando com modificadores, se houver
        if (input_file != NULL)
          freopen(input_file, "r", stdin);
        if (output_file != NULL)
          freopen(output_file, "w", stdout);
        if (pipe_command != NULL) {
          int pipefd[2];
          pipe(pipefd);

          pid_t pipe_pid = fork();
          if (pipe_pid == 0) {
            // processo filho - executa o comando do lado direito do modificador
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            execlp(pipe_command, pipe_command, NULL);
          } else {
            // processo pai - executa o comando do lado esquerdo do modificador
            close(pipefd[0]);
            dup2(pipefd[1], STDIN_FILENO);
            execlp(args[0], args[0], NULL);
          }
        } else {
          // executa o comando normalmente
          execvp(args[0], args);
        }
      } else {
        // processo pai - espera pelo filho terminar
        if (background == 0) wait(NULL);
      }
    }
  }

  return 0;
}
