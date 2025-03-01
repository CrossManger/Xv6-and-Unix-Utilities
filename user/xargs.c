#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define lengthMax 512

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Does not enough input arguments\n");
    exit(1);
  }

  char line[lengthMax];
  char* args[MAXARG];
  int i;

  for (i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }

  while (1) {
    int index = 0;
    char c;

    while (read(0, &c, 1) > 0) {
      if (c == '\n') {
        line[index] = '\0';
        break;
      }
      line[index++] = c;
    }

    if (index == 0) {
      break;
    }

    args[argc - 1] = line;

    args[argc] = 0;

    int pid = fork();
    if (pid < 0) {
      printf("xargs: fork failed\n");
      exit(1);
    }
    else
      if (pid == 0) {
        exec(args[0], args);
        printf("xargs: exec %s failed\n", args[0]);
        exit(1);
      }
      else {
        wait(0);
      }
  }

  exit(0);
}
