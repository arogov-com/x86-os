#include "string.h"

typedef struct _INTERRUPTS
 {
  unsigned long irq_keyboard;
  unsigned long irq_timer;
  unsigned long irq_rtc;
 }INTERRUPTS;

typedef struct _EXCEPTIONS
 {
  unsigned long DE;
  unsigned long DB;
  unsigned long NMI;
  unsigned long BP;
  unsigned long OF;
  unsigned long BR;
  unsigned long UD;
  unsigned long NM;
  unsigned long DF;
  unsigned long CO;
  unsigned long TS;
  unsigned long NP;
  unsigned long SS;
  unsigned long GP;
  unsigned long PF;
  unsigned long MF;
  unsigned long AC;
  unsigned long MC;
  unsigned long XF;
 }EXCEPTIONS;

typedef struct _MEMORY
 {
  unsigned long mem_count;
  unsigned long mem_used;
  unsigned long pages;
 }MEMORY;

typedef struct _SYSCALLS
 {
  unsigned long calls[256];
 }SYSCALLS;

INTERRUPTS interrupts;
EXCEPTIONS exceptions;
MEMORY memory;
SYSCALLS syscalls;

void systat_init(void);
