#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int) __attribute__((noreturn));

void primes(int in_fd) {
  int prime, num;
  int p[2];

  if (read(in_fd, &prime, sizeof(int)) == 0) {
    close(in_fd);
    exit(0);
  }

  printf("prime %d\n", prime);


  if (pipe(p) < 0) {
    printf("Error: Pipe failed\n");
    close(in_fd);
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("Error: Fork failed\n");
    close(in_fd);
    close(p[0]);
    close(p[1]);
    exit(1);
  }
  else if (pid == 0) {
    close(p[1]);
    close(in_fd);

    primes(p[0]);

    close(p[0]);
    exit(0);
  }
  else {
    close(p[0]);

    while (read(in_fd, &num, sizeof(int))) {
      if (num % prime != 0) {
        write(p[1], &num, sizeof(int));
      }
    }

    close(p[1]);
    close(in_fd);

    wait(0);
  }

  exit(0);
}

int main() {
  int p[2];

  if (pipe(p) < 0) {
    printf("Error: Pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("Error: Fork failed\n");
    close(p[0]);
    close(p[1]);
    exit(1);
  }
  else if (pid == 0) {
    close(p[1]);
    primes(p[0]);
    close(p[0]);
    exit(0);
  }
  else {
    close(p[0]);

    for (int i = 2; i <= 280; i++) {
      write(p[1], &i, sizeof(int));
    }

    close(p[1]);
    wait(0);
  }

  exit(0);
}
