#include "syscounters.h"
#include "string.h"
#include "kernel.h"

volatile interrupts_t interrupts;
volatile exceptions_t exceptions;
volatile memory_t memory;
volatile syscalls_t syscalls;
volatile scheduler_t scheduler_perf;

void systat_init(void) {
    memset((void *)&scheduler_perf, 0, sizeof(scheduler_t));
    memset((void *)&interrupts, 0, sizeof(interrupts_t));
    memset((void *)&exceptions, 0, sizeof(exceptions_t));
    memset((void *)&memory, 0, sizeof(memory_t));
    memset((void *)&syscalls, 0, sizeof(syscalls_t));
    memory.mem_used = HEEP_START;
 }
