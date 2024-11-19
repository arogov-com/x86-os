// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stdint.h>
#include "interrupt.h"
#include "ioports.h"

__attribute__ ((naked))
void irq_dummy() {
    asm volatile("pushal");
    asm volatile("movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n");
    asm volatile("popal");
    asm volatile("iret");
}

int set_interrupt(unsigned char vector, void (*func)(), unsigned char type, int dpl, int overwrite) {
    idt_descriptor_t *idt = (idt_descriptor_t*)(IDT_TABLE + (unsigned long)vector * 8);
    if(idt->present == 1 && !overwrite) return 1;
    idt->low_addr = (unsigned long)func & 0x0000FFFF;
    idt->selector = SYS_CODE_SELECTOR;
    idt->reserved = 0;
    idt->type = type;
    idt->reserved2 = 0;
    idt->dpl = dpl;
    idt->present = 1;
    idt->high_addr = (unsigned short)(((unsigned long)func & 0xFFFF0000) >> 16);
    return 0;
}

void *get_interrupt(unsigned char vector) {
    idt_descriptor_t *idt = (idt_descriptor_t*)(IDT_TABLE + vector * 8);
    return (void *)((((unsigned long)idt->high_addr) << 16) | ((unsigned long)idt -> low_addr));
}

void idtr_init(void *address, unsigned short limit) {
    char idtreg[6];                                         //Not array, just fword
    *(unsigned short*)idtreg = limit;                       //Limit 256*sizeof(idt_descriptor_t)-1
    *(unsigned long*)(idtreg + 2) = (unsigned long)address; //Base address of IDT
    asm volatile("lidt 0(,%0,)"::"a"(idtreg));
}

void interrupt_enable() {
    asm volatile("inb $0x70, %al \n andb $0x7f, %al \n outb %al, $0x70 \n sti");
}

void interrupt_disable() {
    asm volatile("cli \n inb $0x70, %al \n orb $0x80, %al \n outb %al, $0x70");
}

int mask_irq(int irq, int enable) {
    if(irq > 15) {
        return 1;
    }
    unsigned short port = PIC1_PORT_DATA;
    unsigned char mask = 1, old;
    if(irq >= 8 && irq <= 15) {
        port = PIC2_PORT_DATA;
    }

    old = inportb(port);
    mask = mask << irq;
    mask = enable ? ((~mask) & old) : (mask | old);
    outportb(port, mask);
    return 0;
}

void init_pic(int pic1_base, int pic2_base) {
    //Base for PIC1
    outportb(0x20, 0x11);
    outportb(0x21, pic1_base);
    outportb(0x21, 4);
    outportb(0x21, 1);
    //Base for PIC2
    outportb(0xA0, 0x11);
    outportb(0xA1, pic2_base);
    outportb(0xA1, 2);
    outportb(0xA1, 1);
    //Enable all lines
    outportb(0x21, 0);
    outportb(0xA1, 0);
}
