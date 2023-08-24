#include "interrupt.h"
#include "ioports.h"
#include "tty.h"

int set_interrupt(unsigned char vector, void (*func)(),unsigned char type,int dpl,int overwrite)
{
 IDT_DESCRIPTOR *idt=(IDT_DESCRIPTOR*)(IDT_TABLE+(unsigned long)vector*8);
 if(idt->present==1 && !overwrite)return 0;
 idt->low_addr=(unsigned long)func & 0x0000FFFF;
 idt->selector=SYS_CODE_SELECTOR;
 idt->reserved=0;
 idt->type=type;
 idt->reserved2=0;
 idt->dpl=dpl;
 idt->present=1;
 idt->high_addr=(unsigned short)(((unsigned long)func & 0xFFFF0000)>>16);
}

void *get_interrupt(unsigned char vector)
{
 IDT_DESCRIPTOR *idt=(IDT_DESCRIPTOR*)(IDT_TABLE+vector*8);
 return (((unsigned long)idt->high_addr)<<16)|((unsigned long)idt->low_addr);
}

void idtr_init(void* address,unsigned short limit)
{
 char idtreg[6];                                     //Not array, just fword
 *(unsigned short*)idtreg=limit;                     //Limit 256*sizeof(IDT_DESCRIPTOR)-1
 *(unsigned long*)(idtreg+2)=(unsigned long)address; //Base address of IDT
 asm("lidt 0(,%0,)"::"a"(idtreg));
}

void interrupt_enable()
{
 asm("inb $0x70,%al \n andb $0x7f,%al \n outb %al,$0x70 \n sti");
}

void interrupt_disable()
{
 asm("cli \n inb $0x70,%al \n orb $0x80,%al \n outb %al,$0x70");
}

int mask_irq(int irq,int enable)
 {
  if(irq>15 || enable>1)return 0;
  unsigned short port=0x21;
  unsigned char mask=1,old;
  if(irq>=8 && irq<=15)port=0xA1;
  old=inportb(port);
  mask=mask<<irq;
  mask=enable?((~mask)&old):(mask|old);
  outportb(port,mask);
 }

void init_pic(int pic1_base,int pic2_base)
 {
  //Base for PIC1
  outportb(0x20,0x11);
  outportb(0x21,pic1_base);
  outportb(0x21,4);
  outportb(0x21,1);
  //Base for PIC1
  outportb(0xA0,0x11);
  outportb(0xA1,pic2_base);
  outportb(0xA1,2);
  outportb(0xA1,1);
  //Enable all lines
  outportb(0x21,0);
  outportb(0xA1,0);
 }
