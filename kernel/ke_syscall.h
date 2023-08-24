#ifdef __WIN32__
 #define SYSCALL_HANDLER(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("push %ecx \n push %edx \n push %ebx \n push %ebp \n push %esi \n push %edi \n call __" #func " \n pop %edi \n pop %esi\n pop %ebp \n pop %ebx \n pop %edx \n pop %ecx \n iret \n");\
  void _ ## func(void)
#else
 #define SYSCALL_HANDLER(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("push %ecx \n push %edx \n push %ebx \n push %ebp \n push %esi \n push %edi \n call _" #func " \n pop %edi \n pop %esi\n pop %ebp \n pop %ebx \n pop %edx \n pop %ecx \n iret \n");\
  void _ ## func(void)
#endif

void syscall_handler(void);
