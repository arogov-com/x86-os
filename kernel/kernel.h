// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#define KERNEL            0x200000  // Kernel base address
#define KERNEL_END        0x2FFFFF  // Kernel base address

#define GDT_ADDR          0x300000  // Global Descriptors Table; Size 64kb (0x10000) = 8192 entries * 8(sizeof descriptor)
#define IDT_TABLE         0x310000  // Interrupt Descriptors Table; Size= 2kb (0x800) = 256entries * 8(sizeof descriptor)

#define VTTY0_BUFF        0x320000
#define VTTY0_VIDEO       0x321000
#define VTTY1_BUFF        0x322000
#define VTTY1_VIDEO       0x323000
#define VTTY2_BUFF        0x324000
#define VTTY2_VIDEO       0x325000
#define VTTY3_BUFF        0x326000
#define VTTY3_VIDEO       0x327000
#define VTTY4_BUFF        0x328000
#define VTTY4_VIDEO       0x329000
#define VTTY5_BUFF        0x32A000
#define VTTY5_VIDEO       0x32B000
#define VTTY6_BUFF        0x32C000
#define VTTY6_VIDEO       0x32D000
#define VTTY7_BUFF        0x32E000
#define VTTY7_VIDEO       0x32F000
#define VTTY8_BUFF        0x320000
#define VTTY8_VIDEO       0x331000
#define VTTY9_BUFF        0x332000
#define VTTY9_VIDEO       0x333000

#define PROCESS_LIST      0x334000
#define MAX_PROC          0x1024

#define PDE_ADDR          0x400000  // Page directory address;    Size = 4kb (0x1000) = 1024entries * 4bytes
#define PTE_ADDR          0x401000  // Page tables start address; Size = 4mb (0x400000) = 1024tables * 1024entries * 4bytes
#define UPMM_ADDR         0x801000  // Used Physical Memory Map;  Size = 128kb (0x20000) = 32 pages per 4GB

#define KAM_ADDR          0x821000  // Kernel allocated memory struct address

#define HEEP_START        0x900000  // Free memory

//void sysrq_handler(unsigned short scancode);

void kernel_main();
