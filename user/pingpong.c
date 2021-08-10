#include "kernel/types.h"
#include "user/user.h"

void close_pipe(int *p) {
  close(p[0]);
  close(p[1]);
}

int main(int argc, char *argv[]) {
  int p2c[2];
  int c2p[2];
  if (pipe(p2c) < 0 || pipe(c2p) < 0) {
    fprintf(2, "pingpong: pipe init failed\n");
    exit(1);
  }
  
  int forkpid = fork();
  if (forkpid == 0) {
    // child
    char c;
    if (read(p2c[0], &c, 1) != 1) {
      fprintf(2, "pingpong: child piperead failed\n");
      close_pipe(p2c);
      close_pipe(c2p);
      exit(1);
    }

    fprintf(1, "%d: received ping\n", getpid());

    if (write(c2p[1], " ", 1) != 1) {
      fprintf(2, "pingpong: child pipewrite failed\n");
      close_pipe(p2c);
      close_pipe(c2p);
      exit(1);
    }

    close_pipe(p2c);
    close_pipe(c2p);
    exit(0);
  } else if (forkpid > 0) {
    // parent 
    if (write(p2c[1], " ", 1) != 1) {
      fprintf(2, "pingpong: parent pipewrite failed\n");
      close_pipe(p2c);
      close_pipe(c2p);
      exit(1);
    }
    
    char c;
    if (read(c2p[0], &c, 1) != 1) {
      fprintf(2, "pingpong: parent piperead failed\n");
      close_pipe(p2c);
      close_pipe(c2p);
      exit(1);
    }

    fprintf(1, "%d: received pong\n", getpid());

    close_pipe(p2c);
    close_pipe(c2p);
    exit(0);
  } else {
    fprintf(2, "pingpong: fork failed\n");
    close_pipe(p2c);
    close_pipe(c2p);
    exit(1);
  }
  
}
