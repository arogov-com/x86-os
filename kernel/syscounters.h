// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stdint.h>
#include "string.h"

typedef struct {
    uint32_t irq_keyboard;
    uint32_t irq_timer;
    uint32_t irq_rtc;
}interrupts_t;

typedef struct {
    uint32_t DE;
    uint32_t DB;
    uint32_t NMI;
    uint32_t BP;
    uint32_t OF;
    uint32_t BR;
    uint32_t UD;
    uint32_t NM;
    uint32_t DF;
    uint32_t CO;
    uint32_t TS;
    uint32_t NP;
    uint32_t SS;
    uint32_t GP;
    uint32_t PF;
    uint32_t MF;
    uint32_t AC;
    uint32_t MC;
    uint32_t XF;
}exceptions_t;

typedef struct {
    uint32_t mem_count;
    uint32_t mem_used;
    uint32_t pages_used;
}memory_t;

typedef struct {
    uint32_t calls[256];
}syscalls_t;

typedef struct {
    uint32_t interrupts, switches;
}scheduler_t;

void systat_init(void);
