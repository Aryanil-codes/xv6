#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_waitx(void)
{
  uint64 addr, addr1, addr2;
  uint wtime, rtime;
  argaddr(0, &addr);
  argaddr(1, &addr1); // user virtual memory
  argaddr(2, &addr2);
  int ret = waitx(addr, &wtime, &rtime);
  struct proc *p = myproc();
  if (copyout(p->pagetable, addr1, (char *)&wtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr2, (char *)&rtime, sizeof(int)) < 0)
    return -1;
  return ret;
}

//added
uint64 sys_getSysCount(void)
{

  int syscall_num;

  argint(0, &syscall_num);

  // printf("%d", syscall_num);

  // Validate that syscall_num is within the correct range
  if (syscall_num < 0 || syscall_num >= 30)
  {
    return -1; // Invalid syscall number
  }

  // Get the current process
  struct proc *p = myproc();

  // Sum syscall counts from this process and all its children
  int total = p->syscall_counts[syscall_num];

  return total;
}

//added for alarmtest

uint64
sys_sigalarm(void)
{
  int interval;
  uint64 handler;

  argint(0, &interval);
  argaddr(1, &handler);
  
  struct proc *p = myproc();
  
  // Disable the alarm if interval is 0
  if (interval == 0) {
    p->alarm_interval = 0;
    p->alarm_handler = 0;
    p->ticks_since_alarm = 0;
    p->alarm_active = 0;
    if (p->alarm_trapframe) {
      kfree(p->alarm_trapframe);
      p->alarm_trapframe = 0;
    }
  } else {
    p->alarm_interval = interval;
    p->alarm_handler = handler;
    p->ticks_since_alarm = 0;
    p->alarm_active = 0;
  }

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  if(p->alarm_trapframe) {
    memmove(p->trapframe, p->alarm_trapframe, sizeof(struct trapframe));
    p->alarm_active = 0;
  }
  return 0;
}

