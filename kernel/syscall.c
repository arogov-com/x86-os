#include "syscall.h"
#include "interrupt.h"
#include "syscounters.h"
#include "vtty.h"
#include "ke_syscall.h"

void syscall(int number, int p1, int p2, int p3) {
    asm volatile("movl %0, %%eax"::"m"(number));
    asm volatile("movl %0, %%edi"::"m"(p1));
    asm volatile("movl %0, %%esi"::"m"(p2));
    asm volatile("movl %0, %%edx"::"m"(p3));
    asm volatile("int $0x80");
}

int read(int fd, const void *buff, unsigned int size) {
    asm volatile("movl %0, %%eax"::"i"(SYS_READ));
    asm volatile("movl %0, %%edi"::"m"(fd));
    asm volatile("movl %0, %%esi"::"m"(buff));
    asm volatile("movl %0, %%edx"::"m"(size));
    asm volatile("int $0x80");
}

int write(int fd, const void *buff, unsigned int size) {
    asm volatile("movl %0, %%eax"::"i"(SYS_WRITE));
    asm volatile("movl %0, %%edi"::"m"(fd));
    asm volatile("movl %0, %%esi"::"m"(buff));
    asm volatile("movl %0, %%edx"::"m"(size));
    asm volatile("int $0x80");
}
