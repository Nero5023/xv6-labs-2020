#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int read_int_from_pipe(int fd) {
  char c;
  char buff[3];
  int idx = 0;
  while (read(fd, &c, 1) == 1) {
    buff[idx] = c;
    idx += 1;
    if (c == '\n') {
      buff[idx-1] = '\0';
      return atoi(buff);
    }
  }
  return -1;
}

void child_process(int* p) { 
    close(p[1]);

    int factor = read_int_from_pipe(p[0]);
    if (factor == -1) {
      return;
    }

    fprintf(1, "prime %d\n", factor); 
    
    int newp[2];
    if (pipe(newp) < 0) {
      fprintf(2, "prime: pipe init failed\n");
      exit(1);
    }
    int pid = fork();
    if (pid == 0) {
      // child
      child_process(newp);
    } else {
      // parent
      close(newp[0]);
      while (1) {
        int num = read_int_from_pipe(p[0]);
        if (num == -1) {
          break;
        }
        if (num % factor == 0) {
          continue;
        }
        fprintf(newp[1], "%d\n", num);
      }
      close(newp[1]);
      wait(0);
      close(p[0]);
    }

}

int main(int argc, char *argv[]) {
  int p[2];
  if (pipe(p) < 0) {
    fprintf(2, "prime: pipe init failed\n");
    exit(1);
  }
  
  int pid = fork();

  if (pid == 0) {
    // child
    child_process(p);
  } else {
    // parent
    close(p[0]);
    int i;
    int factor = 2;
    for (i = 2; i < 36; i++) {
      if (i == factor) {
        fprintf(1, "prime %d\n", i);
      }
      if (i % factor == 0) {
        continue;
      }
      fprintf(p[1], "%d\n", i);
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}
