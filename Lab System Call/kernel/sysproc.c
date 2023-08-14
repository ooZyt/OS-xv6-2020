#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "kernel/sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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

//设置进程的跟踪掩码
uint64
sys_trace(void)
{
  int x;
  if(argint(0,&x)<0)
    return -1;

  //函数接受一个参数作为跟踪掩码，并存在trace_mask中
  myproc()->trace_mask=x;
  return 0;

}

uint64
sys_sysinfo(void)
{
  uint64 addr;
  //检查参数
  if(argaddr(0, &addr) < 0)
    return -1;
  
  struct sysinfo info;
  //获取可用内存
  info.freemem = getFreeMem();
  //获取进程数
  info.nproc = getProcNum();
    
  //传输内容：进程页表，用户态目标地址，数据源地址，数据大小
  if(copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    //若copy失败
    return -1;
  
  return 0;
}
