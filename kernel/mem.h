// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stdint.h>

#define KAM_SIZE 57088

typedef struct {
    uint32_t page;
    void *addr;
    uint32_t size;
    uint32_t pages_used:24;
    uint32_t used:8;
}__attribute__((packed)) kam_t;

void kam_init(void *kam_address, unsigned long size);
unsigned long get_memory_count();
void *kealloc(unsigned long size);
void kefree(void *ptr);
