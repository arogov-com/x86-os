#include "syscall.h"
#include "interrupt.h"
#include "syscounters.h"
#include "vtty.h"
#include "ke_syscall.h"

int syscall(int number, int p1, int p2, int p3) {
    int ret;
    asm volatile("movl %0, %%eax"::"m"(number));
    asm volatile("movl %0, %%edi"::"m"(p1));
    asm volatile("movl %0, %%esi"::"m"(p2));
    asm volatile("movl %0, %%edx"::"m"(p3));
    asm volatile("int $0x80");
    asm volatile("movl %%eax, %0"::"m"(ret));
    return ret;
}

int read(int fd, const void *buff, unsigned int size) {
    int ret;
    asm volatile("movl %0, %%eax"::"i"(SYS_READ));
    asm volatile("movl %0, %%edi"::"m"(fd));
    asm volatile("movl %0, %%esi"::"m"(buff));
    asm volatile("movl %0, %%edx"::"m"(size));
    asm volatile("int $0x80");
    asm volatile("movl %%eax, %0"::"m"(ret));
    return ret;
}

int write(int fd, const void *buff, unsigned int size) {
    int ret;
    asm volatile("movl %0, %%eax"::"i"(SYS_WRITE));
    asm volatile("movl %0, %%edi"::"m"(fd));
    asm volatile("movl %0, %%esi"::"m"(buff));
    asm volatile("movl %0, %%edx"::"m"(size));
    asm volatile("int $0x80");
    asm volatile("movl %%eax, %0"::"m"(ret));
    return ret;
}
