#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

#define SYS_NAME_LEN 8

// Fetch the uint64 at addr from the current process.
int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if(addr >= p->sz || addr+sizeof(uint64) > p->sz)
    return -1;
  if(copyin(p->pagetable, (char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
int
fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();
  int err = copyinstr(p->pagetable, buf, addr, max);
  if(err < 0)
    return err;
  return strlen(buf);
}

static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  *ip = argraw(n);
  return 0;
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
int
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (including nul), -1 if error.
int
argstr(int n, char *buf, int max)
{
  uint64 addr;
  if(argaddr(n, &addr) < 0)
    return -1;
  return fetchstr(addr, buf, max);
}

extern uint64 sys_chdir(void);
extern uint64 sys_close(void);
extern uint64 sys_dup(void);
extern uint64 sys_exec(void);
extern uint64 sys_exit(void);
extern uint64 sys_fork(void);
extern uint64 sys_fstat(void);
extern uint64 sys_getpid(void);
extern uint64 sys_kill(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_mknod(void);
extern uint64 sys_open(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_sleep(void);
extern uint64 sys_unlink(void);
extern uint64 sys_wait(void);
extern uint64 sys_write(void);
extern uint64 sys_uptime(void);
extern uint64 sys_trace(void);

static uint64 (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_trace]   sys_trace,
};

static void sys_call_name(int call_num, char* sys_name) {
  switch (call_num)
  {
  case SYS_fork:
    strncpy(sys_name, "fork", SYS_NAME_LEN);
    break;
  case SYS_exit:
    strncpy(sys_name, "exit", SYS_NAME_LEN);
    break;
  case SYS_wait:
    strncpy(sys_name, "wait", SYS_NAME_LEN);
    break;
  case SYS_pipe:
    strncpy(sys_name, "pipe", SYS_NAME_LEN);
    break;
  case SYS_read:
    strncpy(sys_name, "read", SYS_NAME_LEN);
    break;
  case SYS_kill:
    strncpy(sys_name, "kill", SYS_NAME_LEN);
    break;
  case SYS_exec:
    strncpy(sys_name, "exec", SYS_NAME_LEN);
    break;
  case SYS_fstat:
    strncpy(sys_name, "fstat", SYS_NAME_LEN);
    break;
  case SYS_chdir:
    strncpy(sys_name, "chdir", SYS_NAME_LEN);
    break;
  case SYS_dup:
    strncpy(sys_name, "dup", SYS_NAME_LEN);
    break;
  case SYS_getpid:
    strncpy(sys_name, "getpid", SYS_NAME_LEN);
    break;
  case SYS_sbrk:
    strncpy(sys_name, "sbrk", SYS_NAME_LEN);
    break;
  case SYS_sleep:
    strncpy(sys_name, "sleep", SYS_NAME_LEN);
    break;
  case SYS_uptime:
    strncpy(sys_name, "uptime", SYS_NAME_LEN);
    break;
  case SYS_open:
    strncpy(sys_name, "open", SYS_NAME_LEN);
    break;
  case SYS_write:
    strncpy(sys_name, "write", SYS_NAME_LEN);
    break;
  case SYS_mknod:
    strncpy(sys_name, "mknod", SYS_NAME_LEN);
    break;
  case SYS_unlink:
    strncpy(sys_name, "unlink", SYS_NAME_LEN);
    break;
  case SYS_link:
    strncpy(sys_name, "link", SYS_NAME_LEN);
    break;
  case SYS_mkdir:
    strncpy(sys_name, "mkdir", SYS_NAME_LEN);
    break;
  case SYS_close:
    strncpy(sys_name, "close", SYS_NAME_LEN);
    break;
  case SYS_trace:
    strncpy(sys_name, "trace", SYS_NAME_LEN);
    break;
  default:
    strncpy(sys_name, "UNKNOWN", SYS_NAME_LEN);
    break;
  }
}

void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->trapframe->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->trapframe->a0 = syscalls[num]();
    if ((1 << num & p->trace_mask) != 0) {
      char sys_name[SYS_NAME_LEN];
      sys_call_name(num, sys_name);
      printf("%d: syscall %s -> %d\n", p->pid, sys_name, p->trapframe->a0);
    }
  } else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
