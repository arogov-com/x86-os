#include "syscall.h"
#include "interrupt.h"
#include "syscounters.h"
#include "vtty.h"

void syscall(int number,int p1,int p2,int p3)
 {
  asm("movl %0,%%eax"::"m"(number));
  asm("movl %0,%%ebx"::"m"(p1));
  asm("movl %0,%%ecx"::"m"(p2));
  asm("movl %0,%%edx"::"m"(p3));
  //asm("movl %0,%%edi"::"m"(p2));
  asm("int $0x80");
  asm("leave \n ret");
 }

int read(int fd,const void *buff,unsigned int size)
 {
  asm("movl $3,%eax");
  asm("movl %0,%%ebx"::"m"(fd));
  asm("movl %0,%%ecx"::"m"(buff));
  asm("movl %0,%%edx"::"m"(size));
  asm("int $0x80");
  //asm("leave \n ret");
 }

int write(int fd,const void *buff,unsigned int size)
 {
  asm("movl $4,%eax");
  asm("movl %0,%%ebx"::"m"(fd));
  asm("movl %0,%%ecx"::"m"(buff));
  asm("movl %0,%%edx"::"m"(size));
  asm("int $0x80");
  //asm("leave \n ret");
 }
