#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFF_LEN (512)

int read_line(int fd, char* buff) {
  char c;
  int idx = 0;
  while (read(fd, &c, 1) == 1) {
    if (idx >= BUFF_LEN) {
      fprintf(2, "xargs: read line exec %d\n", BUFF_LEN);
      exit(1);
    }
    buff[idx] = c;
    idx += 1;
    if (c == '\n') {
      buff[idx-1] = '\0';
      return 1;
    }
  }
  return -1;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fprintf(2, "Usage: xargs cmd ...");
    exit(1);
  }
  char** new_args = (char**) malloc((argc+1) * sizeof(char*));
  for (int i = 0; i < argc-1; i++) {
    new_args[i] = argv[i+1];
  }
  while (1) {
    char buff[BUFF_LEN];
    int succ = read_line(0, &buff[0]);
    if (succ == -1) {
      break;
    }
    int pid = fork();
    if (pid == 0) {
      //child
  
      new_args[argc-1] = buff;
      new_args[argc] = 0;
      char* cmd = argv[1];

      exec(cmd, new_args);
      fprintf(2, "xargs: child exec error\n");
    } else {
      wait(0);
    }
  }
  exit(0);
}
