//#define MAX_PROC        0x16

#define STATE_NOT_EXIST 0x0
#define STATE_ACTIVE    0x1
#define STATE_READY     0x2
#define STATE_SLEEP     0x3
#define STATE_IOWAIT    0x4

#define QUANTUM         0x4

struct _CONTEXT
 {
  unsigned long edi,esi,ebp,esp,ebx,edx,ecx,eax,eip,cs,eflags;
 }__attribute__((packed));
typedef struct _CONTEXT CONTEXT;

struct _PROCESS
 {
  int state;
  int quantum;
  int vtty;
  unsigned long times;
  CONTEXT context;
 }__attribute__((packed));
typedef struct _PROCESS PROCESS;

void init_scheduler(void *process_list_addr,unsigned int maxproc);
void *get_process_table(void);
int new_task(void *entry_point,void *stack,int vttyn);
void scheduler(void *esp);

unsigned long get_proc_times(unsigned int pid);
