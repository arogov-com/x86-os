#include "gdt.h"
#include "string.h"

gdtr_t gdtr;
char gdt_reg[6];

void lgdt(void *address, unsigned short limit) {
    gdtr_t gdtr;
    gdtr.limit = limit;
    gdtr.address = address;
    asm volatile("lgdt 0(,%0,)"::"a"(&gdtr));
}

void flush_gdt(int code_segment, int data_segment) {
    asm("mov %0, %%ax"::"m"(data_segment));
    asm("mov %ax, %ds");
    asm("mov %ax, %es");
    asm("mov %ax, %fs");
    asm("mov %ax, %gs");
    asm("mov %ax, %ss");
    asm("jmp $0x08, $flush_gdt2 \n flush_gdt2:");
}

int set_descriptor(void *gdt_addr, unsigned short num, void *base, unsigned long limit, int present, int dpl, int db, int granularity, int sys, int type) {
    if(num > GDT_SIZE) {
        return 0;
    }
    gdtr_t *gdtr;
    seg_descr_t *gdt;
    if(!gdt_addr) {
        asm volatile("sgdt gdt_reg");
        gdtr = (gdtr_t*)gdt_reg;
        gdt = gdtr->address;
    }
    else gdt = (seg_descr_t*)gdt_addr;

    gdt[num].limit_low = (uint16_t)limit;
    gdt[num].base_low = (uint32_t)base;
    gdt[num].type = type;
    gdt[num].sys = sys;
    gdt[num].dpl = dpl;
    gdt[num].present = present;
    gdt[num].limit_high = (limit >> 16) & 0xf;
    gdt[num].avl = 0;
    gdt[num].reserved = 0;
    gdt[num].db = db;
    gdt[num].granularity = granularity;
    gdt[num].high_base = (uint8_t)(((uint32_t)base) >> 24);
    return 1;
}

int get_gdt(int num, seg_descr_t *store) {
    gdtr_t *gdtr;
    asm volatile("sgdt gdt_reg");
    gdtr = (gdtr_t*) gdt_reg;
    if(num * sizeof(seg_descr_t) > gdtr->limit + 1) {
        return 0;
    }
    seg_descr_t *gdt = gdtr->address;
    memcpy(store, &gdt[num], sizeof(seg_descr_t));
    return 1;
}
