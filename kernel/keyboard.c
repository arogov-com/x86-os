#include "keyboard.h"
#include "virtualkey.h"
#include "ioports.h"
#include "interrupt.h"
#include "syscounters.h"
#include "kernel.h"
#include "vtty.h"

extern interrupts_t interrupts;

volatile int irq_keyboard_state;
volatile unsigned short scancode;
volatile unsigned char e0_e1, sysrq;
volatile int led_state;

volatile char num_lock;
volatile char caps_lock;
volatile char scroll_lock;

void keyboard_init() {
    irq_keyboard_state = 0;
    scancode = 0;
    e0_e1 = 0;
    sysrq = 0;
    led_state = 0;

    num_lock = 1;
    caps_lock = 0;
    scroll_lock = 0;

    set_led(LED_SCROLL | LED_NUM | LED_CAPS, 0);
    set_led(LED_NUM, 1);
    //outportb(0x60,0xF2);
}

IRQ_HANDLER(irq_keyboard) {
    ++interrupts.irq_keyboard;
    unsigned char creg, sc;
    /*
    while(inportb(0x64)&2);
    outportb(0x64,0xAD);
    while(inportb(0x64)&2);
    */
    sc = inportb(0x60);
    creg = inportb(0x61);
    outportb(0x61, creg | 0x80);
    outportb(0x61, creg);

    if(sc == 0xE0 || sc == 0xE1) {
        e0_e1 = sc;
        return;
    }

    if(e0_e1) {
        scancode = (((unsigned short)e0_e1) << 8) | sc;
        e0_e1 = 0;

        if(scancode == VK_SYS_RQ_MAKE) {
            return;
        }
        else if(scancode == VK_SYS_RQ_REPEAT) {
            sysrq = 1;
            return;
        }
        else if(scancode == VK_SYS_RQ_BREAK) {
            sysrq = 0;
            return;
        }
        if(sysrq) {
            sysrq_handler(scancode);
            return;
        }
        irq_keyboard_state = 1;
        return;
    }

    scancode = sc;

    if(scancode == VK_NUM_LOCK) {
        if(num_lock) {
            num_lock = 0;
            set_led(LED_NUM, 0);
        }
        else {
            num_lock = 1;
            set_led(LED_NUM, 1);
        }
    }
    else if(scancode == VK_SCROLL_LOCK) {
        if(scroll_lock) {
            scroll_lock = 0;
            set_led(LED_SCROLL, 0);
        }
        else {
            scroll_lock = 1;
            set_led(LED_SCROLL, 1);
        }
    }
    else if(scancode == VK_CAPS) {
        if(caps_lock) {
            caps_lock = 0;
            set_led(LED_CAPS, 0);
        }
        else {
            caps_lock = 1;
            set_led(LED_CAPS, 1);
        }
    }
    vtty_handler(scancode);
    /*
    while(inportb(0x64)&2);
    outportb(0x64, 0xAE);
    while(inportb(0x64)&2);
    */
    irq_keyboard_state = 1;
}

void keyboard_wait(void) {
    irq_keyboard_state = 0;
    while(!irq_keyboard_state);
//  irq_keyboard_state=0;
}

unsigned short readkey(void) {
    irq_keyboard_state = 0;
    while(!irq_keyboard_state);
//  irq_keyboard_state=0;
    return scancode;
}

void set_autorepeat(char opt) {
    opt |= 0x80;
    while(inportb(0x64) & 2);
    outportb(0x60, 0xF3);
    while(inportb(0x64) & 2);
    outportb(0x60, opt);
}

void set_led(int led, int state) {
    if(state) led |= led_state;
    else led = (~led) & led_state;
    led &= 7;
    while(inportb(0x64) & 2);
    outportb(0x60, 0xED);
    while(inportb(0x64) &2);
    outportb(0x60, (unsigned char)led);
}

int get_num_state(void) {
    return num_lock;
}

int get_caps_state(void) {
    return caps_lock;
}

int get_scroll_state(void) {
    return scroll_lock;
}
