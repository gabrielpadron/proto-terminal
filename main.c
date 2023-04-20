#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h> // define wait
#include <unistd.h>   // fork()

#define MAX_LENGTH 1024 // tamanho máximo do comando

void execute_ls() {
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(".")) != NULL) {
    // imprime o nome de todos os arquivos e diretorios do diretorio atual
    while ((ent = readdir(dir)) != NULL) {
      printf("%s\n", ent->d_name);
    }
    closedir(dir);
  } else {
    perror("Não foi possível abrir o diretório");
  }
}

void execute_cd(char *path) {
  if (chdir(path) != 0) {
    perror("Não foi possível mudar para o diretório especificado");
  }
}

void execute_pwd() {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("Não foi possível obter o diretório atual");
  }
}

void execute_cmd(char **cmd_args) {
  if (strcmp(cmd_args[0], "cd") == 0) {
    execute_cd(cmd_args[1]);
  } else if (strcmp(cmd_args[0], "pwd") == 0) {
    execute_pwd();
  } else if (strcmp(cmd_args[0], "ls") == 0) {
    execute_ls();
  } else {
    execvp(cmd_args[0], cmd_args);
  }
}

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
          execute_cmd(args);
        }
      } else {
        // processo pai - espera pelo filho terminar
        if (background == 0) wait(NULL);
      }
    }
  }

  return 0;
}
