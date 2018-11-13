#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  struct proc *curproc = myproc();
  struct systemcall getpid_syscall ;
  getpid_syscall.syscall_number = 11;
  // getpid_syscall.syscall_name = "getpid";
  getpid_syscall.syscall_name[6] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&getpid_syscall.time);
  getpid_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = getpid_syscall;
  curproc->number_of_systemcalls++;
  return curproc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *curproc = myproc();
  struct systemcall sbrk_syscall ;
  sbrk_syscall.syscall_number = 12;
  // sbrk_syscall.syscall_name = "sbrk";
  sbrk_syscall.syscall_name[4] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&sbrk_syscall.time);
  sbrk_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = sbrk_syscall;
  curproc->number_of_systemcalls++;


  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  if(argint(0, &n) < 0)
    return -1;

  struct proc *curproc = myproc();
  struct systemcall sleep_syscall;
  sleep_syscall.syscall_number = 13;
  // sleep_syscall.syscall_name = "sleep";
  sleep_syscall.syscall_name[5] = '\0';
  //sleep_syscall.time = malloc(sizeof(struct rtcdate));
  cmostime(&sleep_syscall.time);
  sleep_syscall.caller_pid = curproc->pid;
  //sleep_syscall.argumants = malloc(sizeof(char * 4));
  //sleep_syscall->argumants = "void";
  //sleep_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = sleep_syscall;
  curproc->number_of_systemcalls++;

 
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  struct proc *curproc = myproc();
  struct systemcall uptime_syscall ;
  uptime_syscall.syscall_number = 14;
  // uptime_syscall.syscall_name = "uptime";
  uptime_syscall.syscall_name[6] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&uptime_syscall.time);
  uptime_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = uptime_syscall;
  curproc->number_of_systemcalls++;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int sys_inc_num(void)
{
  struct proc *curproc = myproc();
  int before; 
  int s;

  if(argint(0, &s) < 0)
    return -1;
  before = curproc->tf->eax;

  curproc->tf->eax = s;
  inc_num(s);
  
  curproc->tf->eax = before;
  return 0;
}
int sys_invoked_syscalls(void)
{
    int pid;
    //int result; 
    struct proc *curproc = myproc();
    struct systemcall invoked_syscalls_syscall;
    invoked_syscalls_syscall.syscall_number = 23;
    // invoked_syscalls_syscall.syscall_name = "invoked_syscalls";
    invoked_syscalls_syscall.syscall_name[16] = '\0';
    //sleep_syscall.time = malloc(sizeof(struct rtcdate));
    cmostime(&invoked_syscalls_syscall.time);
    invoked_syscalls_syscall.caller_pid = curproc->pid;
    //sleep_syscall.argumants = malloc(sizeof(char * 4));
    //sleep_syscall->argumants = "void";
    //sleep_syscall->argumants[4] = '\0';
    curproc->syscalls[curproc->number_of_systemcalls] = invoked_syscalls_syscall;
    curproc->number_of_systemcalls++;

    if(argint(0, &pid) < 0)
      return -1;
    invoked_syscalls(pid);
    return 0;


}
