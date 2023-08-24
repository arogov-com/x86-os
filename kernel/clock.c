#include "clock.h"
#include "interrupt.h"
#include "syscounters.h"
#include "ioports.h"
#include "task.h"

extern interrupts_t interrupts;
extern exceptions_t exceptions;
extern memory_t memory;
extern syscalls_t syscalls;
extern scheduler_t scheduler_perf;

IRQ_HANDLER(irq_timer) {
    ++interrupts.irq_timer;
    void *esp;
    asm volatile("mov %%esp, %0": "=r" (esp));
    scheduler(esp + 0x20);
}

IRQ_HANDLER(RTC) {
    ++interrupts.irq_rtc;
    asm volatile("movb $0xC, %al");
    asm volatile("outb %al, $0x70");
    asm volatile("inb $0x71, %al");
}

void set_i8254_counter(unsigned short counter) {
    outportb(PIT_CMD_PORT, PIT_CH0_CMD | PIT_ACC_HILO | PIT_MODE3 | PIT_BIN);
    outportb(PIT_CH0_PORT, (char)counter);
    outportb(PIT_CH0_PORT, counter >> 8);
}

void set_i8254_freq(unsigned short freq) {
    unsigned short counter = PIT_FREQ / freq;
    outportb(PIT_CMD_PORT, PIT_CH0_CMD | PIT_ACC_HILO | PIT_MODE3 | PIT_BIN);
    outportb(PIT_CH0_PORT, (char)counter);
    outportb(PIT_CH0_PORT, counter >> 8);
}

void set_rtc_interval(char interval) {
    outportb(0x70, 0xa);                                 // Select register
    uint8_t state = inportb(0x71);                       // Get state
    outportb(0x71, ((state | 0x80) & 0xF0) | interval);  // Set state
}

void enable_rtc_interrupt(int enable) {
    outportb(0x70, 0xb);
    uint8_t state = inportb(0x71);
    outportb(0x71, enable ? (state | 0x40) : (state & 0xbf));
}

void timer_tick(unsigned long ticks) {
    ticks += interrupts.irq_timer;                 //Overflow handled
    while(interrupts.irq_timer <= ticks);
}

void rtc_tick(unsigned long ticks) {
    ticks += interrupts.irq_rtc;                   //Overflow handled
    while(interrupts.irq_rtc <= ticks);
}

void gettime(datetime_t *dt) {
    uint8_t p;
    outportb(0x70, 0xB);
    p = inportb(0x71);
    p |= 0x6;
    outportb(0x71, p);

    outportb(0x70, 0x0);
    dt->second = inportb(0x71);

    outportb(0x70, 0x2);
    dt->minute = inportb(0x71);

    outportb(0x70, 0x4);
    dt->hour = inportb(0x71);

    outportb(0x70, 0x7);
    dt->day = inportb(0x71);

    outportb(0x70, 0x8);
    dt->month = inportb(0x71);

    outportb(0x70, 0x9);
    dt->year = inportb(0x71);
}
