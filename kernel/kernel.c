// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stdint.h>
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
#include "task.h"
#include "ke_syscall.h"
#include "virtualkey.h"
#include "stdio.h"
#include "acpi.h"
#include "multiboot.h"

#define BUFFER_SIZE 512

extern scheduler_t scheduler_perf;
char buff[BUFFER_SIZE];

void kernel_thread();

void panic(char* str) {
    set_vtty_attribute(FOREGROUND_GRAY | LIGHT);
    printf("Kernel panic: %s\n", str);
    while(1);
 }

void sysrq_handler(unsigned short scancode) {
    if(scancode == VK_ALT) {
        *(char*)(VIDEO_RAM_DEFAULT + 60 * 2) = 'Q';
    }
    if(scancode == VK_ALT) {
        *(char*)(VIDEO_RAM_DEFAULT + 60 * 2) = ' ';
    }
}

void heli_thread() {
    char h[] = "|/-\\";
    int i;
    while(1) {
        for(i = 0; i != 4; ++i) {
            *(char*)(VIDEO_RAM_DEFAULT + 70 * 2) = h[i];
            timer_tick(100);
        }
    }
}

void heli2_thread() {
    char h[] = "|\\-/";
    int i;
    while(1) {
        for(i = 0; i != 4; ++i) {
            *(char*)(VIDEO_RAM_DEFAULT + 71 * 2) = h[i];
            timer_tick(100);
        }
    }
}

multiboot_header_t multiboot_header __attribute__((section(".multiboot_header"))) = {0xe85250d6, 0x0, 0x00000018, 0x17adaf12, 0x0, 0x00000008};

// __attribute__ ((naked))
void kernel_main() {
    int i, blen;

    systat_init();

    init_vttys();
    mk_vtty(0, (void*)VTTY0_VIDEO, (void*)VTTY0_BUFF, 0x1000);
    mk_vtty(1, (void*)VTTY1_VIDEO, (void*)VTTY1_BUFF, 0x1000);
    mk_vtty(2, (void*)VTTY2_VIDEO, (void*)VTTY2_BUFF, 0x1000);
    mk_vtty(3, (void*)VTTY3_VIDEO, (void*)VTTY3_BUFF, 0x1000);
    mk_vtty(4, (void*)VTTY4_VIDEO, (void*)VTTY4_BUFF, 0x1000);
    mk_vtty(5, (void*)VTTY5_VIDEO, (void*)VTTY5_BUFF, 0x1000);
    mk_vtty(6, (void*)VTTY6_VIDEO, (void*)VTTY6_BUFF, 0x1000);
    mk_vtty(7, (void*)VTTY7_VIDEO, (void*)VTTY7_BUFF, 0x1000);
    mk_vtty(8, (void*)VTTY8_VIDEO, (void*)VTTY8_BUFF, 0x1000);
    mk_vtty(9, (void*)VTTY9_VIDEO, (void*)VTTY9_BUFF, 0x1000);
    switch_vtty(0);
    set_vtty_attribute(FOREGROUND_GRAY | LIGHT);
    clrscr();

    blen = snprintf(buff, BUFFER_SIZE, "ASDF OS 0.1\n\nMemory count %i bytes\n", get_memory_count());
    vtty_write(0, buff, blen);

    memset((void*)GDT_ADDR, 0, GDT_SIZE * sizeof(seg_descr_t));
    set_descriptor((void*)GDT_ADDR, 0, 0, 0, 0, 0, 0, 0, 1, 0);                    // Null descriptor
    set_descriptor((void*)GDT_ADDR, 1, 0, 0, 0, 0, 0, 0, 1, 0);                    // Null descriptor
    set_descriptor((void*)GDT_ADDR, 2, 0, 0xFFFFFF, 1, 0, 1, 1, 1, CODE_READABLE); // System code descriptor
    set_descriptor((void*)GDT_ADDR, 3, 0, 0xFFFFFF, 1, 0, 1, 1, 1, DATA_WRITABLE); // System data descriptor
    lgdt((void*)GDT_ADDR, GDT_SIZE);

    blen = snprintf(buff, BUFFER_SIZE, "Page directory address: %X | Page tables start address: %X\n", PDE_ADDR, PTE_ADDR);
    memset((void*)UPMM_ADDR, 0, 0x20000);
    pde_init();
    for(i = 0; i != 0x900000; i += 0x1000) {
        if(!assign_page((void*)i, (void*)i, 1, 0)) {
            panic("can\'t init system memory\n");
        }
    }
    vtty_write(0, buff, blen);
    vtty_write(0, "Enable page translation\n", sizeof("Enable page translation\n"));
    paging_enable(1);
    kam_init((void *)KAM_ADDR, KAM_SIZE * sizeof(kam_t));

    vtty_write(0, "Initialize PIC\n", sizeof("Initialize PIC\n"));
    init_pic(0x20, 0x28);

    blen = snprintf(buff, BUFFER_SIZE, "Initialize interrupts:\n\
 Timer(IRQ0, Vector 0x20): 0x%x\n\
 Keyboard(IRQ1, Vector 0x21): 0x%x\n\
 Floppy (IRQ6, Vector 0x26): 0x%x\n\
 Real Time Clock(IRQ8, Vector 0x28): 0x%x\n", irq_timer, irq_keyboard, irq_fdc, RTC);
    vtty_write(0, buff, blen);

    memset((void*)IDT_TABLE, 0x0, 256 * 8);
    for(i = 0x20; i != 0x2F; ++i) {
        set_interrupt(i, irq_dummy, TYPE_INTERRUPT, 0, 1);
    }
    set_interrupt(0x20, irq_timer, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x21, irq_keyboard, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x26, irq_fdc, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x28, RTC, TYPE_INTERRUPT, 0, 1);

    blen = snprintf(buff, BUFFER_SIZE, "System call: 0x%X\n", syscall_handler);
    vtty_write(0, buff, blen);
    syscalls_init();
    set_interrupt(0x80, syscall_handler, TYPE_TRAP, 3, 1);

    blen = snprintf(buff, BUFFER_SIZE, "Initialize scheduler. PROCESS list addr: %X\n", PROCESS_LIST);
    vtty_write(0, buff, blen);
    init_scheduler((void *)PROCESS_LIST, MAX_PROC);

    blen = snprintf(buff, BUFFER_SIZE, "Initialize exceptions\n\
 DE : 0x%X  |  TS : 0x%X  |  DB : 0x%X\n\
 NP : 0x%X  |  NMI: 0x%X  |  SS : 0x%X\n\
 BP : 0x%X  |  GP : 0x%X  |  OF : 0x%X\n\
 PF : 0x%X  |  BR : 0x%X  |  MF : 0x%X\n\
 UD : 0x%X  |  AC : 0x%X  |  NM : 0x%X\n\
 MC : 0x%X  |  DF : 0x%X  |  XF : 0x%X\n\
 CO : 0x%X  |\n", DE, TS, DB, NP, NMI, SS, BP, GP, OF, PF, BR, MF, UD, AC, NM, MC, DF, XF, CO);
    vtty_write(0, buff, blen);
    init_exceptions();

    blen = snprintf(buff, BUFFER_SIZE, "Initialize IDT\n IDT address: 0x%X\n Limit: 0x%X\n", IDT_TABLE, 256 * 8 - 1);
    vtty_write(0, buff, blen);
    idtr_init((void*)IDT_TABLE, 256 * 8 - 1);

    // printf("Set RTC interval to 1024Hz\n");
    // set_rtc_interval(RTC_IRQ8_1024HZ);
    // printf("Enable RTC\n");
    // enable_rtc_interrupt(1);

    printf("Set system timer freq 1000 Hz\n");
    set_i8254_freq(1000);

    printf("Enable interrupts in PIC\n");
    interrupt_enable();

    printf("Initialize keyboard\n");
    keyboard_init();

    printf("Find ACPI:\n");
    rsdp_t *rsdp = acpi_find_rsdp((void *)RSDP_ADDR_START, (void *)RSDP_ADDR_END);
    if(!rsdp) {
        printf(" RSDP is not found\n");
    }
    else if(acpi_chk_summ(rsdp, sizeof(rsdp_t))) {
        printf(" Bad checksumm\n");
    }
    else {
        assign_page((void *)rsdp->rsdt_ptr, (void *)rsdp->rsdt_ptr, 0, 0);
        printf(" RSDP: %X\n RSDT address: %X\n Version: %i\n", rsdp, rsdp->rsdt_ptr, rsdp->version);

        rsdt_t *rsdt = (rsdt_t *)rsdp->rsdt_ptr;
        printf(" DTs count: %i\n", (rsdt->header.length - sizeof(rsdt->header)) / 4);
        acpi_process_rsdt(rsdt);
    }

    void *kernel_thread_stack = kealloc(0x1000);
    kthread(kernel_thread, kernel_thread_stack, 0);
    while(1);
}

void kernel_thread() {
    printf("Kernel thread has started\n");

    void *shell_stack = kealloc(0x1000);
    int pid = kthread(shell, shell_stack, 0);
    printf("Shell thread started PID = %X\n", pid);

    void *helli_stack = kealloc(0x1000);
    pid = kthread(heli_thread, helli_stack, 0);
    printf("Heli thread started PID = %X\n", pid);

    void *heli2_stack = kealloc(0x1000);
    pid = kthread(heli2_thread, heli2_stack, 0);
    printf("Heli2 thread started PID = %X\n", pid);

    while(1);
}
