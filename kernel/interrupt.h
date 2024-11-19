// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stdint.h>
#include "kernel.h"

#define IRQ_HANDLER(func) __attribute__ ((naked)) void func() {asm volatile("pushal"); _##func(); asm volatile("movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n mov 0xC(%esp), %eax \n sub $0x20, %eax \n mov %eax, 0xC(%esp) \n mov %eax, %esp \n popal \n iret \n");} void _##func()
#define EXCEPTION_HANDLER(func) __attribute__ ((naked)) void func() {asm volatile("pushal"); _##func(); asm volatile("popal \n iret \n");} void _##func()
#define EXCEPTION_HANDLER_ERRCODE(func) __attribute__ ((naked)) void func() {asm volatile("pop errorcode \n pushal"); _##func(); asm volatile("popal \n iret \n");} void _##func()

#define SYS_CODE_SELECTOR 0x10

#define TYPE_CALL         0x5
#define TYPE_INTERRUPT    0xE
#define TYPE_TRAP         0xF

// 8259 PIC constants
#define PIC1_PORT_CMD     0x20
#define PIC1_PORT_DATA    0x21
#define PIC2_PORT_CMD     0xA0
#define PIC2_PORT_DATA    0xA1
#define PIC_EOI           0x20

typedef struct {
    uint16_t low_addr;
    uint32_t selector:16;
    uint32_t reserved:8;
    uint32_t type:4;
    uint32_t reserved2:1;
    uint32_t dpl:2;         //Descriptor Privelege Level
    uint32_t present:1;
    uint16_t high_addr;
}idt_descriptor_t;

void irq_dummy(void);

int set_interrupt(unsigned char vector, void (*func)(), unsigned char type, int dpl, int overwrite);
void *get_interrupt(unsigned char vector);
void idtr_init(void *address, unsigned short limit);
void interrupt_enable();
void interrupt_disable();

int mask_irq(int irq, int enable);
void init_pic(int pic1_base, int pic2_base);
