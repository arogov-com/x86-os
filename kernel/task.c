#include "kernel.h"
#include "task.h"

PROCESS *process;
int MAX_PROC;
int current;

void init_scheduler(void *process_list_addr,unsigned int maxproc)
 {
  MAX_PROC=maxproc;
  process=(PROCESS*)process_list_addr;
  memset(process,0,sizeof(PROCESS)*MAX_PROC);
  current=0;
 }

void *get_process_table(void)
 {
  return &process;
 }

int new_task(void *entry_point,void *stack,int vttyn)
 {
  int i;
  asm("cli");
  for(i=0;i!=MAX_PROC;++i)
   {
    if(process[i].state==STATE_NOT_EXIST)
     {
      process[i].context.edi=0;
      process[i].context.esi=0;
      process[i].context.ebp=stack;
      process[i].context.esp=stack;
      process[i].context.ebx=0;
      process[i].context.edx=0;
      process[i].context.ecx=0;
      process[i].context.eax=0;
      process[i].context.eip=(unsigned long)entry_point;
      process[i].context.cs=0x8;
      process[i].context.eflags=process[0].context.eflags;

      process[i].state=STATE_READY;
      process[i].quantum=QUANTUM;
      process[i].vtty=vttyn;
      process[i].times=0;
      asm("sti");
      return i;
     }
   }
  asm("sti");
  return 0;
 }

void scheduler(void *esp)
 {
  if(process[0].state==STATE_NOT_EXIST)
   {
    process[0].state=STATE_IOWAIT;
    process[0].quantum=QUANTUM;
    process[0].vtty=0;
    process[0].times=0;
    memcpy(&process[0].context,esp,sizeof(CONTEXT));
    return;
   }
  if(process[current].quantum)
   {
    --process[current].quantum;
    process[current].times++;
    return;
   }
  else
   {
    int i,tmp;
    for(i=0,tmp=current;i!=MAX_PROC-1;++i)
     {
      ++tmp;
      if(tmp==MAX_PROC)tmp=0;
      if(process[tmp].state==STATE_READY)
       {
        process[current].state=STATE_READY;
        memcpy(&process[current].context,esp,sizeof(CONTEXT));

        memcpy(esp,&process[tmp].context,sizeof(CONTEXT));
        process[tmp].state=STATE_ACTIVE;
        process[tmp].quantum=QUANTUM;
        current=tmp;
        return;
       }
     }
    process[current].quantum=QUANTUM;
    return;
   }
 }

unsigned long get_proc_times(unsigned int pid)
 {
  return process[pid].times;
 }
