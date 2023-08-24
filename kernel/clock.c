#include "clock.h"
#include "tty.h"
#include "interrupt.h"
#include "syscounters.h"
#include "ioports.h"
#include "task.h"
/*
IRQ_HANDLER(irq_timer)
 {
  scheduler();
  ++interrupts.irq_timer;
 }
*/
asm(".globl _irq_timer");
asm(".def _irq_timer; .scl 2; .type 32; .endef");
asm("_irq_timer:");
asm("pusha");
asm("push %esp");
asm("call _scheduler");
asm("pop %esp");
asm("movl _interrupts+4,%eax \n incl %eax \n movl %eax, _interrupts+4");
asm("movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n");
asm("popa \n iret");

IRQ_HANDLER(RTC)
 {
  ++interrupts.irq_rtc;
  asm("movb $0xC,%al");
  asm("outb %al,$0x70");
  asm("inb $0x71,%al");
 }

void set_rtc_interval(char interval)
 {
  unsigned char state;
  asm("movb $0xA,%al");    //Select register
  asm("outb %al,$0x70");
  asm("inb $0x71,%%al":"=a"(state):);    //Get state
  state=((state|0x80)&0xF0)|interval;    //Set state
  asm("outb %b0,$0x71"::"a"(state));
 }

void enable_rtc_interrupt(int enable)
 {
/*
  unsigned char pic2=inportb(0xA0);
  outportb(0xA1,pic2|1);
*/
  asm("movb $0x0B,%al");   //Select register
  asm("outb %al,$0x70");

  unsigned char state;
  asm("inb $0x71,%%al":"=a"(state):);      //Get state
  state|=0xC0;
  asm("outb %b0,$0x71"::"a"(state));
  
  //outportb(0xA1,pic2);
 }

void timer_tick(unsigned long ticks)
 {
  ticks+=interrupts.irq_timer;               //Overflow handled
  while(interrupts.irq_timer!=ticks);
 }
void rtc_tick(unsigned long ticks)
 {
  ticks+=interrupts.irq_rtc;                 //Overflow handled
  while(interrupts.irq_rtc!=ticks);
 }

void gettime(DATETIME *dt)
 {
  unsigned char p;
  outportb(0x70,0xB);
  p=inportb(0x71);
  p=p|0x6;
  outportb(0x71,p);

  outportb(0x70,0x0);
  dt->second=inportb(0x71);

  outportb(0x70,0x2);
  dt->minute=inportb(0x71);

  outportb(0x70,0x4);
  dt->hour=inportb(0x71);
 }
