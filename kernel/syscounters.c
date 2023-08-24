#include "syscounters.h"
#include "string.h"
#include "kernel.h"

void systat_init(void)
 {
  memset(&interrupts,0,sizeof(INTERRUPTS));
  memset(&exceptions,0,sizeof(EXCEPTIONS));
  memset(&memory,0,sizeof(MEMORY));
  memset(&syscalls,0,sizeof(SYSCALLS));
  memory.mem_used=HEEP_START;
 }

