#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE 512
#define MAX_ARGS MAXARG

int my_strcmp(char* s1, char* s2) {
  while (*s1 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int my_atoi(char* s) {
  int n = 0;
  while (*s >= '0' && *s <= '9') {
    n = n * 10 + (*s - '0');
    s++;
  }
  return n;
}

int main(int argc, char* argv[]) {
  char* args[MAX_ARGS];
  int arg_count = 0;
  int n_value = 0;
  int command_index = 1;

  if (argc < 2) {
    printf("Usage: xargs command [args...]\n");
    exit(1);
  }

  if (argc >= 3 && my_strcmp(argv[1], "-n") == 0) {
    n_value = my_atoi(argv[2]);
    if (n_value <= 0) {
      printf("xargs: invalid number: %s\n", argv[2]);
      exit(1);
    }
    command_index = 3;
  }

  for (int i = 0; i < argc - command_index; i++) {
    args[i] = argv[i + command_index];
    arg_count++;
  }

  char line[MAX_LINE];
  char ch;
  int line_len = 0;

  while (1) {
    line_len = 0;

    int bytes_read;
    while ((bytes_read = read(0, &ch, 1)) > 0) {
      if (ch == '\n') break;
      if (line_len < MAX_LINE - 1) {
        line[line_len++] = ch;
      }
    }

    line[line_len] = '\0';

    if (bytes_read == 0) {
      if (line_len == 0) {
        break;
      }
    }

    if (line_len == 0) {
      continue;
    }

    char* token_start = line;
    int token_pos = 0;
    char* tokens[MAX_ARGS];
    int token_count = 0;

    while (token_pos < line_len) {
      while (token_pos < line_len && (line[token_pos] == ' ' || line[token_pos] == '\t')) {
        line[token_pos] = '\0';
        token_pos++;
      }

      if (token_pos >= line_len) break;

      token_start = line + token_pos;
      tokens[token_count++] = token_start;

      while (token_pos < line_len && line[token_pos] != ' ' && line[token_pos] != '\t') {
        token_pos++;
      }
    }

    if (token_count == 0) continue;

    if (n_value > 0) {
      for (int i = 0; i < token_count; i += n_value) {
        int pid = fork();
        if (pid < 0) {
          printf("xargs: fork failed\n");
          exit(1);
        }

        if (pid == 0) {
          for (int j = 0; j < arg_count; j++) {
            args[j] = argv[j + command_index];
          }

          int tokens_to_add = (i + n_value < token_count) ? n_value : (token_count - i);
          for (int j = 0; j < tokens_to_add; j++) {
            args[arg_count + j] = tokens[i + j];
          }

          args[arg_count + tokens_to_add] = 0;

          exec(args[0], args);
          printf("xargs: exec %s failed\n", args[0]);
          exit(1);
        }
        else {
          wait(0);
        }
      }
    }
    else {
      for (int i = 0; i < token_count; i++) {
        if (arg_count + i < MAX_ARGS - 1) {
          args[arg_count + i] = tokens[i];
        }
      }

      args[arg_count + token_count] = 0;

      int pid = fork();
      if (pid < 0) {
        printf("xargs: fork failed\n");
        exit(1);
      }

      if (pid == 0) {
        exec(args[0], args);
        printf("xargs: exec %s failed\n", args[0]);
        exit(1);
      }
      else {
        wait(0);
      }
    }
  }

  exit(0);
}
