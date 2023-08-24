#include "kernel.h"

#ifdef __WIN32__
 #define IRQ_HANDLER(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("pusha \n call __" #func "\n movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n popa \n iret \n");\
  void _ ## func(void)
 #define EXCEPTION_HANDLER(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("pusha \n call __" #func "\n popa \n iret \n");\
  void _ ## func(void)
 #define EXCEPTION_HANDLER_ERRCODE(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("pop _errorcode \n pusha \n call __" #func "\n popa \n iret \n");\
  void _ ## func(void)
#else
 #define IRQ_HANDLER(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("pusha \n call _" #func "\n movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n popa \n iret \n");\
  void _ ## func(void)
 #define EXCEPTION_HANDLER(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("pusha \n call _" #func "\n popa \n iret \n");\
  void _ ## func(void)
 #define EXCEPTION_HANDLER_ERRCODE(func) asm(".globl _" #func " \n	.def	_" #func ";	.scl	2;	.type	32;	.endef \n _" #func ":\n");\
  asm("pop errorcode \n pusha \n call _" #func "\n popa \n iret \n");\
  void _ ## func(void)
#endif
/*
#ifdef __WIN32__
 #define IRQ_HANDLER(func) void func(void)\
  {asm("pop %ebp \n pusha \n call __" #func " \n movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n popa \n iret \n");}\
  void _ ## func(void)
 #define EXCEPTION_HANDLER(func) void func(void)\
  {asm("pop %ebp \n pusha \n call __" #func " \n popa \n iret \n");}\
  void _ ## func(void)
 #define EXCEPTION_HANDLER_ERRCODE(func) void func(void)\
  {asm("pop %ebp \n pop _errorcode \n pusha \n call __" #func " \n popa \n iret \n");}\
  void _ ## func(void)
#else
 #define IRQ_HANDLER(func) void func(void)\
  {asm("pop %ebp \n pusha \n call _" #func " \n movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n popa \n iret \n");}\
  void _ ## func(void)
 #define EXCEPTION_HANDLER(func) void func(void)\
  {asm("pop %ebp \n pusha \n call _" #func " \n popa \n iret \n");}\
  void _ ## func(void)
 #define EXCEPTION_HANDLER_ERRCODE(func) void func(void)\
  {asm("pop %ebp \n pop errorcode \n pusha \n call _" #func " \n popa \n iret \n");}\
  void _ ## func(void)
#endif
*/
#define SYS_CODE_SELECTOR 0x8

#define TYPE_CALL         0xC
#define TYPE_INTERRUPT    0xE
#define TYPE_TRAP         0xF

typedef struct _IDT_DESCRIPTOR
 {
  unsigned short low_addr;
  unsigned selector:16;
  unsigned reserved:8;
  unsigned type:4;
  unsigned reserved2:1;
  unsigned dpl:2;         //Descriptor Privelege Level
  unsigned present:1;
  unsigned short high_addr;
 }IDT_DESCRIPTOR;

int set_interrupt(unsigned char vector, void (*func)(),unsigned char type,int dpl,int overwrite);
void idtr_init(void* address,unsigned short limit);
void interrupt_enable();
void interrupt_disable();

int mask_irq(int irq,int enable);
void init_pic(int pic1_base,int pic2_base);
