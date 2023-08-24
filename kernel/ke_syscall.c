#include "ke_syscall.h"
#include "syscounters.h"

SYSCALL_HANDLER(syscall_handler)
 {
  int number,p1,p2,p3;
  asm("movl %%eax,%0"::"m"(number));
  asm("movl %%ebx,%0"::"m"(p1));
  asm("movl %%ecx,%0"::"m"(p2));
  asm("movl %%edx,%0"::"m"(p3));
  ++syscalls.calls[0];
  switch(number)
   {
    case 3:++syscalls.calls[3];
           sys_read(p1,p2,p3);
           break;
    case 4:++syscalls.calls[4];
           sys_write(p1,p2,p3);
           break;
    default:printf("Unknown system call %i",number);
   }
 }

int sys_write(int fd,const void *buff,int count)
 {
  if(fd==0 || fd==2)return -1;
  if(fd==1)return vtty_write(-1,buff,count);
  return 0;
 }

int sys_read(int fd,void *buff,int count)
 {
  if(fd==1 || fd==2)return -1;
  if(fd==0)return vtty_read();
 }
