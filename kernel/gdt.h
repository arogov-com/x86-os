// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stdint.h>

#define CODE                     0xb  // 1 1000 0xb
#define CODE_READABLE            0xa  // 1 1010 0xa
#define CODE_CONFORMING          0xc  // 1 1100 0xc
#define CODE_CONFORMING_READABLE 0xe  // 1 1110 0xe

#define DATA                     0x0  // 1 0000 0x0
#define DATA_WRITABLE            0x2  // 1 0010 0x2
#define DATA_STACK               0x4  // 1 0100 0x4
#define DATA_STACK_WRITABLE      0x6  // 1 0110 0x6

#define TASK_SEGMENT_INACTIVE    0x9
#define TASK_SEGMENT_BUSY        0x11

#define SYS_DESCR                0x08
#define GDT_SIZE                 8192

typedef struct {
    uint16_t limit;
    void *address;
}__attribute__((packed)) gdtr_t;

typedef struct {             // 5.2 page 3159
    uint32_t limit_low:16;   // Low 16 bits of limit
    uint32_t base_low:24;    // Low 24 bits of base
    uint32_t type:4;
        // uint32_t accessed:1; // This bit is set to 1 by hardware when the segment is accessed, and cleared by software
        // uint32_t rw:1;       // Data: 0 - the data segment may be read but not written to. Code: 0 - the segment may be executed but not read from.
        // uint32_t ec:1;       // Data: 0 - the segment expands from base address up to base+limit. 1 - it expands from maximum offset down to limit, a behavior usually used for stacks. Code: Code in this segment may be called from less-privileged levels.
        // uint32_t type:1;     // 0 - Data segment, 1 - code/sys segment
    uint32_t sys:1;          // 0 - System segment, 1 - Data/Code segment
    uint32_t dpl:2;          // Descriptor Privilege Level
    uint32_t present:1;      // 0 - Segment is not present , 1 - Segment is present
    uint32_t limit_high:4;   // High 4 bits of limit
    uint32_t avl:1;          // Available for system software, not used by hardware
    uint32_t reserved:1;     // Should be 0
    uint32_t db:1;           // D(code) OpSz 0 - 16bit, 1 - 32bit; B(data) SegSz 0 - 0xffff, 1 - 0xffffffff; Ignore for sys seg
    uint32_t granularity:1;  // Granularity 0/1 - 1 bit/4096 bits
    uint32_t high_base:8;    // High 8 bits of base
}__attribute__((packed)) seg_descr_t;

// Load Global Descriptors Table Registor
void lgdt(void *address, unsigned short limit);

// Set GDT item num; If gdt_addr==0 then GDT address take from sgdt; YOU MUST DISABLE INTERRUPTS MANUALY
int set_descriptor(void *gdt_addr, unsigned short num, void *base, unsigned long limit, int present, int dpl, int db, int granularity, int sys, int type);

// Store GDT item num to store
int get_gdt(int num, seg_descr_t *store);
