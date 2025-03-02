#include "kernel/types.h"
#include "user/user.h"

int main() {
  int parent[2], child[2]; // Phần tử 0 là đầu đọc, phần tử 1 là đầu ghi
  char buf;

  if (pipe(parent) < 0 || pipe(child) < 0) {
    fprintf(2, "Error: cannot create pipes\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "Error: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Tiến trình con:
    close(parent[1]);
    close(child[0]);

    if (read(parent[0], &buf, 1) != 1) {
      fprintf(2, "Child: read failed\n");
      close(parent[0]);
      close(child[1]);
      exit(1);
    }
    // In ra thông báo với PID của tiến trình con
    printf("%d: received ping\n", getpid());

    if (write(child[1], &buf, 1) != 1) {
      fprintf(2, "Child: write failed\n");
      close(parent[0]);
      close(child[1]);
      exit(1);
    }

    // Đóng các đầu ống và thoát
    close(parent[0]);
    close(child[1]);
    exit(0);
  }
  else {
    // Tiến trình cha:
    close(parent[0]);
    close(child[1]);

    // Ghi một byte vào pipe gửi cho tiến trình con
    buf = 'm';
    if (write(parent[1], &buf, 1) != 1) {
      fprintf(2, "Parent: write failed\n");
      close(parent[1]);
      close(child[0]);
      exit(1);
    }

    if (read(child[0], &buf, 1) != 1) {
      fprintf(2, "Parent: read failed\n");
      close(parent[1]);
      close(child[0]);
      exit(1);
    }
    // In ra thông báo với PID của tiến trình cha
    printf("%d: received pong\n", getpid());

    // Đóng các đầu ống và chờ tiến trình con kết thúc
    close(parent[1]);
    close(child[0]);
    wait(0);
    exit(0);
  }
}
