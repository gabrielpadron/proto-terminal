#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_FILES 1000


void execute_cd(char *path) {
  if (chdir(path) != 0) {
    perror("");
  }
}

void execute_pwd() {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("");
  }
}

void sort(char *arr[], int n) {
  char *temp;

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (strcmp(arr[j], arr[j + 1]) > 0) {
        temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}

void execute_ls() {
  DIR *dir;
  struct dirent *ent;
  char *files[MAX_FILES];
  int count = 0;

  if ((dir = opendir(".")) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      files[count] = strdup(ent->d_name);
      count++;
    }
    closedir(dir);

    sort(files, count);

    for (int i = 0; i < count; i++) {
      struct stat file_stat;
      if (stat(files[i], &file_stat) == 0) {
        if (S_ISDIR(file_stat.st_mode))
          printf("%s/\n", files[i]);
        else
          printf("%s\n", files[i]);
      }
      free(files[i]);
    }
  } else {
    perror("");
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
  char command[MAX_COMMAND_LENGTH];

  while (1) {
    execute_pwd();
    printf("$ ");
    fgets(command, MAX_COMMAND_LENGTH, stdin);

    command[strcspn(command, "\n")] = 0;
    if (strcmp(command, "exit") == 0) exit(0);

    char *args[MAX_COMMAND_LENGTH];
    int arg_count = 0;
    char *token = strtok(command, " ");
    char *input_file = NULL;
    char *output_file = NULL;
    char *pipe_command = NULL;
    int background = 0;

    while (token != NULL) {
      if (strcmp(token, "<") == 0) {
        token = strtok(NULL, " ");
        input_file = token;
      } else if (strcmp(token, ">") == 0) {
        token = strtok(NULL, " ");
        output_file = token;
      } else if (strcmp(token, "|") == 0) {
        token = strtok(NULL, " ");
        pipe_command = token;
      } else if (strcmp(token, "&") == 0) {
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

      pid_t pid = fork();

      if (pid == 0) {
        if (input_file != NULL)
          freopen(input_file, "r", stdin);
        if (output_file != NULL)
          freopen(output_file, "w", stdout);
        if (pipe_command != NULL) {
          int pipefd[2];
          pipe(pipefd);

          pid_t pipe_pid = fork();
          if (pipe_pid == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            execlp(pipe_command, pipe_command, NULL);
          } else {
            close(pipefd[0]);
            dup2(pipefd[1], STDIN_FILENO);
            execlp(args[0], args[0], NULL);
          }
        } else {
          execute_cmd(args);
        }
      } else {
        if (background == 0) wait(NULL);
      }
    }
  }

  return 0;
}
