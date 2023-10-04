#include "ke_syscall.h"
#include "syscounters.h"
#include "vtty.h"
#include "syscall.h"
#include "task.h"

extern syscalls_t syscalls;

int (*sys_calls_addr[SYSCALL_NUMBER])();

int sys_write(int fd, const void *buff, int count);
int sys_read(int fd, void *buff, int count);

SYSCALL_HANDLER(syscall_handler) {
    int number, p1, p2, p3;
    asm volatile("movl %%eax, %0"::"m"(number));
    asm volatile("movl %%edi, %0"::"m"(p1));
    asm volatile("movl %%esi, %0"::"m"(p2));
    asm volatile("movl %%edx, %0"::"m"(p3));

    int ret = -1;
    if(number < SYSCALL_NUMBER && sys_calls_addr[number]) {
        ++syscalls.calls[number];
        ret = sys_calls_addr[number](p1, p2, p3);
        asm volatile("movl %%eax, %0"::"m"(ret));
        return;
    }
    asm volatile("movl %%eax, %0"::"m"(ret));
}

void syscalls_init() {
    memset(sys_calls_addr, 0, sizeof(sys_calls_addr));
    sys_calls_addr[SYS_READ] = sys_read;
    sys_calls_addr[SYS_WRITE] = sys_write;
}

int reg_syscall(int num, void *addr, int owerwrite) {
    if(num > SYSCALL_NUMBER - 1 || (sys_calls_addr[num] && !owerwrite)) {
        return -1;
    }
    sys_calls_addr[num] = (int(*)())addr;
    return 1;
}

int sys_write(int fd, const void *buff, int count) {
    if(fd == STDOUT) {
        return vtty_write(get_current_proc_vtty(), buff, count);
    }
    return -1;
}

int sys_read(int fd, void *buff, int count) {
    if(fd == STDIN) {
        return vtty_read(buff, count);
    }
    return -1;
}
