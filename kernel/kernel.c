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

extern scheduler_t scheduler_perf;

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

char buff[512];

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

    blen = sprintf(buff, "ASDF OS 0.1\n\nMemory count %i bytes\n", get_memory_count());
    vtty_write(0, buff, blen);

    memset((void*)GDT_ADDR, 0, GDT_SIZE * sizeof(seg_descr_t));
    set_descriptor((void*)GDT_ADDR, 0, 0, 0, 0, 0, 0, 0, 1, 0);                      // Null descriptor
    set_descriptor((void*)GDT_ADDR, 1, 0, 0, 0, 0, 0, 0, 1, 0);                      // Null descriptor
    set_descriptor((void*)GDT_ADDR, 2, 0, 0xFFFFFF, 1, 0, 1, 1, 1, CODE_READABLE); // System code descriptor
    set_descriptor((void*)GDT_ADDR, 3, 0, 0xFFFFFF, 1, 0, 1, 1, 1, DATA_WRITABLE); // System data descriptor
    set_descriptor((void*)GDT_ADDR, 4, (void *)PROCESS_LIST, sizeof(gdt_tss_t), 1, 0, 0, 0, 0, TASK_SEGMENT_INACTIVE);
    lgdt((void*)GDT_ADDR, 0x2000);

    blen = sprintf(buff, "Page directory address: %X | Page tables start address: %X\n", PDE_ADDR, PTE_ADDR);
    memset((void*)UPMM_ADDR, 0, 0x20000);
    pde_init();
    for(i = 0; i != 0x1000000; i += 0x1000){
        if(!assign_page((void*)i, (void*)i, 1, 0)) {
            panic("can\'t init system memory\n");
        }
    }
    vtty_write(0, buff, blen);
    vtty_write(0, "Enable page translation\n", sizeof("Enable page translation\n"));
    paging_enable(1);
    kam_init();

    vtty_write(0, "Initialize PIC\n", sizeof("Initialize PIC\n"));
    init_pic(0x20, 0x28);

    blen = sprintf(buff, "Initialize interrupts:\n\
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

    blen = sprintf(buff, "System call: 0x%X\n", syscall_handler);
    vtty_write(0, buff, blen);
    set_interrupt(0x80, syscall_handler, TYPE_TRAP, 3, 1);
    syscalls_init();

    blen = sprintf(buff, "Initialize scheduler. PROCESS list addr: %X\n", PROCESS_LIST);
    vtty_write(0, buff, blen);
    init_scheduler((void *)PROCESS_LIST, MAX_PROC);

    blen = sprintf(buff, "Initialize exceptions\n\
 DE : 0x%X  |  TS : 0x%X  |  DB : 0x%X\n\
 NP : 0x%X  |  NMI: 0x%X  |  SS : 0x%X\n\
 BP : 0x%X  |  GP : 0x%X  |  OF : 0x%X\n\
 PF : 0x%X  |  BR : 0x%X  |  MF : 0x%X\n\
 UD : 0x%X  |  AC : 0x%X  |  NM : 0x%X\n\
 MC : 0x%X  |  DF : 0x%X  |  XF : 0x%X\n\
 CO : 0x%X  |\n", DE, TS, DB, NP, NMI, SS, BP, GP, OF, PF, BR, MF, UD, AC, NM, MC, DF, XF, CO);
    vtty_write(0, buff, blen);
    init_exceptions();

    blen = sprintf(buff, "Initialize IDT\n IDT address: 0x%X\n Limit: 0x%X\n", IDT_TABLE, 256 * 8 - 1);
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
    rsdp_t *rsdp = (rsdp_t *)find_rsdp((void *)RSDP_ADDR_START, (void *)RSDP_ADDR_END);
    if(!rsdp) {
        printf("  RSDP is not found\n");
    }
    else {
        printf("  RSDP: %X\n  RSDT: %X\n  Version: %i\n", rsdp, rsdp->rsdt_ptr, rsdp->version);
    }

    kthread(kernel_thread, (void *)0x900000, 0);
    while(1);
}

void kernel_thread() {
    printf("Kernel thread has started\n");

    int pid = kthread(shell, (void *)0xA00000, 0);
    printf("Shell thread started PID = %X\n", pid);

    pid = kthread(heli2_thread, (void *)0xB00000, 0);
    printf("Heli thread started PID = %X\n", pid);

    pid = kthread(heli_thread, (void *)0xC00000, 0);
    printf("Heli2 thread started PID = %X\n", pid);

    timer_tick(5000);

    kill_proc(pid);

    while(1);
}
