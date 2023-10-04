#include <stdint.h>

#define STATE_NOT_EXIST 0x0
#define STATE_ACTIVE    0x1
#define STATE_READY     0x2
#define STATE_SLEEP     0x3
#define STATE_IOWAIT    0x4

#define QUANTUM         0x0a
#define TASK_SEGMENT    0x10

typedef uint32_t reg;

typedef struct {
    reg edi, esi;
    void *ebp, *esp;
    reg ebx, edx, ecx, eax;
    void *eip;
    reg cs, eflags;
} __attribute__((packed)) context_t;

typedef struct process_t {
    uint32_t id;
    uint32_t state;
    uint32_t quantum;
    uint32_t vtty;
    uint64_t times;
    context_t context;
    struct process_t *right, *left;
} __attribute__((packed)) process_t;

typedef struct { // 8.2.1, page 3259
    uint32_t prev; // Previous task link field — Contains the segment selector for the TSS of the previous task
    uint32_t esp0; // Stack pointer when we change to kernel mode
    uint32_t ss0;  // Stack segment when we change to kernel mode
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt; // LDT segment selector field — Contains the segment selector for the task's LDT.
    uint16_t trap;
    uint16_t iopb;
    uint32_t ssp;
} __attribute__((packed)) gdt_tss_t;

void init_scheduler(void *process_list_addr, unsigned int maxproc);
void *get_process_table(void);
int kthread(void *entry_point, void *stack, int vttyn);
void scheduler(void *esp);

int get_proc_struct(int pid, process_t *store);
int get_proc_times(unsigned int pid);

int get_current_proc_vtty();
int kill_proc(int pid);
