#include "kernel.h"

typedef struct {
    unsigned long page;
    unsigned long addr;
    unsigned long size;
    unsigned pages_used:24;
    unsigned reserved:8;
}kam_t;

void kam_init(void);
unsigned long get_memory_count();
void *kealloc(unsigned long size);
