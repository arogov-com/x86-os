#include "kernel.h"

typedef struct _KAM
 {
  unsigned long page;
  unsigned long addr;
  unsigned long size;
  unsigned pages_used:24;
  unsigned reserved:8;
 }KAM;

void kam_init(void);
unsigned long get_memory_count();
void *kealloc(unsigned long size);
