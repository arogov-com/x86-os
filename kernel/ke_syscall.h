#define SYS_READ  3
#define SYS_WRITE 4

#define SYSCALL_HANDLER(func) __attribute__ ((naked)) void func() {asm("pushal"); _##func(); asm("popal \n iret");} void _##func()

void syscall_handler(void);
void syscalls_init();
int reg_syscall(int num, void *addr, int owerwrite);
