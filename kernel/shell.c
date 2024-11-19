// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

// #include "tty.h"
#include "vtty.h"
#include "interrupt.h"
#include "keyboard.h"
#include "ioports.h"
#include "paging.h"
#include "floppy.h"
#include "clock.h"
#include "string.h"
#include "exceptions.h"
#include "mem.h"
#include "syscounters.h"
#include "cpu.h"
#include "shell.h"
#include "kernel.h"
#include "gdt.h"
#include "virtualkey.h"
#include "syscall.h"
#include "task.h"
#include "stdio.h"

extern interrupts_t interrupts;
extern exceptions_t exceptions;
extern memory_t memory;
extern syscalls_t syscalls;
extern scheduler_t scheduler_perf;

void shell() {
    // asm("sti");
    int i;
    char c[32];
    set_vtty_attribute(FOREGROUND_GRAY);
    for(;;) {
        puts("ASDF OS# ");
        gets(c);
        if(!strcmp(c, "clear")) {
            clrscr();
        }
        if(!strcmp(c, "write")) {
            int writen = write(1, "Hello!\n", 7);
            printf("%i bytes writen\n", writen);
        }
        if(!strcmp(c, "led")) {
            set_led(LED_SCROLL | LED_NUM | LED_CAPS, 0);
        }
        if(!strcmp(c, "fdd1")) {
            puts("Start engene...");
            if(fdc_drive_engene(1, 1)) {
                puts("Success\n");
            }
            else {
                puts("fault\n");
            }
        }
        if(!strcmp(c, "fdd2")) {
            puts("Searching track...");
            if(fdc_search_track(0)) {
                puts("Success\n");
            }
            else {
                puts("fault\n");
            }
        }
        if(!strcmp(c, "floppy")) {
            floppy_reset(0);
            printf("OK\n");
        }
        if(!strcmp(c, "kbd"))
        while(1) {
            unsigned short sc = readkey();
            if(sc==VK_ESC) {
                break;
            }
            printf("KEY: %X\n", (unsigned short)sc);
        }
        if(!strcmp(c, "pf")) {
            *(unsigned long*)0x900000 = 1;
        }
        if(!strcmp(c, "dbg")) {
            asm("int $0x3");
        }
        if(strstr(c, "rtc")) {
            char *s = strchr(c, ' ');
            if(s) {
                ++s;
                int num = atoi(s);
                rtc_tick(num);
                putchar('\n');
            }
        }
        if(strstr(c, "timer")) {
            char *s = strchr(c, ' ');
            if(s) {
                ++s;
                int num = atoi(s);
                timer_tick(num);
                putchar('\n');
            }
        }
        if(!strcmp(c, "video")) {
            assign_page((void*)0x900000, (void*)0xB8000, 1, 0);
            assign_page((void*)0x901000, (void*)0xB9000, 1, 0);
            assign_page((void*)0x902000, (void*)0xBA000, 1, 0);
            set_vtty_ram_addr((void*)0x900000);
        }
        if(!strcmp(c,"gmem")) {
            printf("0x%X\n", get_physical_address((void *)0x900000));
        }
        if(!strcmp(c, "gv")) {
            unsigned long asdf[5];
            memset(asdf, 0, sizeof(asdf));
            get_virtual_addresses((void *)0xB8000, asdf);
            printf("%X %X\n", asdf[0], asdf[1]);
        }
        if(!strcmp(c, "da")) {
            free_page((void *)0x900000);
        }

        if(!strcmp(c,"m")) {
            kealloc(469);
        }
        if(!strcmp(c, "m1")) {
            kealloc(46549);
        }
        if(!strcmp(c, "m2")) {
            kealloc(21324);
        }
        if(!strcmp(c, "m3")) {
            kealloc(541020);
        }
        if(!strcmp(c, "m4")) {
            kealloc(1228800);
        }
        if(!strcmp(c, "upmm")) {
            unsigned long *UPMM=(unsigned long*)UPMM_ADDR;
            int j, n = 24;
            for(i = 0; i != 4096; ++i) {
                if(i > n) {
                    n += 24;
                    if(readkey() == VK_ESC) {
                        putchar('\n');
                        break;
                    }
                }
                printf("\n%i-> ",i);
                for(j=0; j!=8; ++j) {
                    printf("%X ",UPMM[(i<<3)+j]);
                }
            }
        }
        if(strstr(c, "pde")) {
            char *s = strchr(c, ' ') +1;
            if(s) {
                int num = atoi(s);
                page_dir_t pd;
                get_pde_entry(num, &pd);
                printf("PRES: %i\nWRIT: %i\nUSR: %i\nADDR: 0x%X\n", pd.present, pd.writable, pd.user, pd.address << 12);
            }
        }
        if(strstr(c, "pte")) {
            char *s = strchr(c, ' ') + 1;
            if(s) {
                char *s1 = strchr(s, ' ');
                if(s1) {
                    *s1 = 0;
                    ++s1;
                    int table = atoi(s);
                    page_table_t pt;
                    int page = atoi(s1);
                    get_pte_entry(table, page, &pt);
                    printf("TABLE: %i   PAGE: %i\nPRES: %i\nWRIT: %i\nUSR: %i\nADDR: 0x%X\n", table,page, pt.present, pt.writable, pt.user, pt.address << 12);
                }
            }
        }
        if(strstr(c, "int")) {
            char *s = strchr(c, ' ') + 1;
            if(s) {
                int num = atoi(s);
                switch(num) {
                    case 0: asm("int $0x0\n");
                            break;
                    case 1: asm("int $0x1\n");
                            break;
                    case 2: asm("int $0x2\n");
                            break;
                    case 3: asm("int $0x3\n");
                            break;
                    case 4: asm("int $0x4\n");
                            break;
                    case 5: asm("int $0x5\n");
                            break;
                    case 6: asm("int $0x6\n");
                            break;
                    case 7: asm("int $0x7\n");
                            break;
                    case 8: asm("int $0x8\n");
                            break;
                    case 9: asm("int $0x9\n");
                            break;
                    case 0xA: asm("int $0xA\n");
                              break;
                    case 0xB: asm("int $0xB\n");
                              break;
                    case 0xC: asm("int $0xC\n");
                              break;
                    case 0xD: asm("int $0xD\n");
                              break;
                    case 0xE: asm("int $0xE\n");
                              break;
                    case 0xF: asm("int $0xF\n");
                              break;
                    case 0x10: asm("int $0x10\n");
                               break;
                    case 0x11: asm("int $0x11\n");
                               break;
                    case 0x12: asm("int $0x12\n");
                               break;
                    case 0x13: asm("int $0x13\n");
                               break;
                    case 0x14: asm("int $0x14\n");
                               break;
                    case 0x15: asm("int $0x15\n");
                               break;
                }
            }
        }
        if(!strcmp(c, "spd")) {
            puts("SPD\n");
            set_autorepeat(REP_SPEED_30 | REP_WAIT_250);
        }
        if(!strcmp(c, "mt")) {
            if(!assign_page((void*)0x900000, (void*)0xB8000, 1, 0)) {
                puts("Allready assigned\n");
            }
        }
        if(!strcmp(c, "fp")) {
            printf("First freepage: %i\n", find_free_page_sq(300));
        }
        if(!strcmp(c, "systat")) {
            printf("Interrupts\n Keyboard: %i\n Timer: %i\n RTC: %i\n", interrupts.irq_keyboard, interrupts.irq_timer, interrupts.irq_rtc);
            printf("Memory\n Total: %i\n Used: %i\n Pages used: %i\n", memory.mem_count, memory.mem_used, memory.pages_used);
        }
        if(!strcmp(c, "idt")) {
            printf("sizeof(idt_descriptor_t)=%i\n", sizeof(idt_descriptor_t));
        }
        if(!strcmp(c, "interrupt")) {
            printf("Interrupt IRQ0:%X\n", get_interrupt(0x20));
        }
        if(strstr(c, "gdt")) {
            char *s=strchr(c,' ');
            if(s) {
                ++s;
                int num = atoi(s);
                seg_descr_t gdt;
                if(get_gdt(num, &gdt))
                printf("Descriptor %i\n Limit=%X\n", num, (((unsigned long)gdt.limit_high) << 16) | gdt.limit_low);
            }
        }
        if(!strcmp(c, "lgdt")) {
            set_descriptor(0, 1, 0x0, 0xFFFFFF, 1, 0, 1, 1, 1, CODE_READABLE);
        }
        if(!strcmp(c, "syscall")) {
            syscall(1, 0xAAAAAAAA, 0xBBBBBBBB, 0xDDDDDDDD);
        }
        if(!strcmp(c, "perf")) {
            printf("interrupts.irq_keyboard = %i\ninterrupts.irq_rtc = %i\ninterrupts.irq_timer = %i\n\n", interrupts.irq_keyboard, interrupts.irq_rtc, interrupts.irq_timer);
            printf("memory.mem_count = %i\nmemory.mem_used = %i\nmemory.pages_used = %i\n\n", memory.mem_count, memory.mem_used, memory.pages_used);
            printf("syscalls.calls[0] = %i\nsyscalls.calls[1] = %i\n\n", syscalls.calls[0], syscalls.calls[1]);
            printf("scheduler_perf.interrupts = %i\nscheduler_perf.switches = %i\n", scheduler_perf.interrupts, scheduler_perf.switches);
        }
        if(strstr(c, "hex")) {
            char *s = strchr(c, ' ');
            if(s) {
                ++s;
                char *s1 = strchr(s, ' ');
                if(s1) {
                    *s1 = 0;
                    ++s1;
                    unsigned long start = hextoint(s, 0);
                    unsigned long stop = hextoint(s1, 0);
                    if(start > stop) {
                        unsigned long i;
                        printf("Memory dump from %X to %X\nAddress | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n", start, stop);
                        while(start != stop) {
                            printf("%s | ",itohex(start, s, 1));
                            for(i = start;i != start + 0x10; ++i) {
                                puts(itohex(*(unsigned char*)i, s, 1));
                                putchar(' ');
                            }
                            putchar('\n');
                            start += 0x10;
                        }
                    }
                    else {
                        puts("Invalid range\n");
                    }
                }
            }
        }

    }
}
