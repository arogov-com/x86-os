#include "tty.h"
#include "vtty.h"
#include "interrupt.h"
#include "keyboard.h"
#include "ioports.h"
#include "paging.h"
#include "floppy.h"
#include "clock.h"
#include "string.h"
#include "exceptions.h"
#include "mem.h"
#include "syscounters.h"
#include "cpu.h"
#include "shell.h"
#include "kernel.h"
#include "gdt.h"
#include "ke_syscall.h"
#include "virtualkey.h"

void panic(char* str)
 {
  setattribute(FOREGROUND_GRAY|LIGHT);
  printf("Kernel panic: %s",str);
  for(;;);
 }
/*
void sysrq_handler(unsigned short scancode)
 {
  if(scancode==VK_ALT)puts("System request\n");
  else puts("SYSRQ");;
 }
*/
void kernel_main()
 {
  int i,blen;
  char buff[512];
  systat_init();

  init_vttys();
  
  mk_vtty(0,(void*)VTTY0_VIDEO,(void*)VTTY0_BUFF,0x1000);
  mk_vtty(1,(void*)VTTY1_VIDEO,(void*)VTTY1_BUFF,0x1000);
  mk_vtty(2,(void*)VTTY2_VIDEO,(void*)VTTY2_BUFF,0x1000);
  mk_vtty(3,(void*)VTTY3_VIDEO,(void*)VTTY3_BUFF,0x1000);
  mk_vtty(4,(void*)VTTY4_VIDEO,(void*)VTTY4_BUFF,0x1000);
  mk_vtty(5,(void*)VTTY5_VIDEO,(void*)VTTY5_BUFF,0x1000);
  mk_vtty(6,(void*)VTTY6_VIDEO,(void*)VTTY6_BUFF,0x1000);
  mk_vtty(7,(void*)VTTY7_VIDEO,(void*)VTTY7_BUFF,0x1000);
  mk_vtty(8,(void*)VTTY8_VIDEO,(void*)VTTY8_BUFF,0x1000);
  mk_vtty(9,(void*)VTTY9_VIDEO,(void*)VTTY9_BUFF,0x1000);
  switch_vtty(0);
  set_vtty_attribute(FOREGROUND_GRAY|LIGHT);
  clrscr();

  blen=sprintf(buff,"ASDF OS 0.1\n\nMemory count %i bytes\n\n",get_memory_count());
  vtty_write(0,buff,blen);
  
  blen=sprintf(buff,"Page directory address: %X | Page tables start address: %X\n",PDE_ADDR,PTE_ADDR);
  memset((void*)UPMM_ADDR,0,0x20000);
  pde_init();
  for(i=0;i!=0x900000;i+=0x1000)if(!assign_page((void*)i,(void*)i,1,0))panic("can\'t init system memory\n");

  vtty_write(0,buff,blen);
  vtty_write(0,"Enable page translation\n",sizeof("Enable page translation\n"));
  paging_enable(1);
  kam_init();

  vtty_write(0,"Initializing PIC\n",sizeof("Initializing PIC\n"));
  init_pic(0x20,0x28);

  blen=sprintf(buff,"Initializing interrupts:\n Timer(IRQ0, Vector 0x20): 0x%x\n Keyboard(IRQ1, Vector 0x21): 0x%x\n Floppy (IRQ6, Vector 0x26): 0x%x\n Real Time Clock(IRQ8, Vector 0x28): 0x%x\n\n",irq_timer,irq_keyboard,irq_fdc,RTC);
  vtty_write(0,buff,blen);

  memset((void*)IDT_TABLE,0x0,256*8);
  set_interrupt(0x20,irq_timer,TYPE_INTERRUPT,0,1);
  set_interrupt(0x21,irq_keyboard,TYPE_INTERRUPT,0,1);
  set_interrupt(0x26,irq_fdc,TYPE_INTERRUPT,0,1);
  set_interrupt(0x28,RTC,TYPE_INTERRUPT,0,1);

  blen=sprintf(buff,"System call: 0x%X\n",syscall_handler);
  vtty_write(0,buff,blen);
  set_interrupt(0x80,syscall_handler,TYPE_TRAP,3,1);

  blen=sprintf(buff,"Initializing scheduler. PROCESS list addr: %X\n",PROCESS_LIST);
  vtty_write(0,buff,blen);
  init_scheduler(PROCESS_LIST,16);

  blen=sprintf(buff,"Initializing exceptions\n\
 DE : 0x%X   |   TS : 0x%X\n\
 DB : 0x%X   |   NP : 0x%X\n\
 NMI: 0x%X   |   SS : 0x%X\n\
 BP : 0x%X   |   GP : 0x%X\n\
 OF : 0x%X   |   PF : 0x%X\n\
 BR : 0x%X   |   MF : 0x%X\n\
 UD : 0x%X   |   AC : 0x%X\n\
 NM : 0x%X   |   MC : 0x%X\n\
 DF : 0x%X   |   XF : 0x%X\n\
 CO : 0x%X   |   \n\n",DE,TS,DB,NP,NMI,SS,BP,GP,OF,PF,BR,MF,UD,AC,NM,MC,DF,XF,CO);
  vtty_write(0,buff,blen);
  init_exceptions();

  blen=sprintf(buff,"Initializing IDT\n IDT address: 0x%X\n Limit: 0x%X\n\n",IDT_TABLE,256*8-1);
  vtty_write(0,buff,blen);
  idtr_init((void*)IDT_TABLE,256*8-1);

  vtty_write(0,"Setting RTC interval to 1024Hz\n",sizeof("Setting RTC interval to 1024Hz\n"));
  set_rtc_interval(RTC_IRQ8_1024HZ);
  vtty_write(0,"Enable RTC\n",sizeof("Enable RTC\n"));
  enable_rtc_interrupt(1);

  vtty_write(0,"Enabling interrupts in PIC\n\n",28);
  interrupt_enable();

  set_descriptor(GDT_ADDR,0,0,0,0,0,0,0,0);                   // Null descriptor
  set_descriptor(GDT_ADDR,1,0,0xFFFFF,1,0,1,1,CODE_READABLE); // System code descriptor
  set_descriptor(GDT_ADDR,2,0,0xFFFFF,1,0,1,1,DATA_WRITABLE); // System data descriptor
  lgdt(GDT_ADDR,0xFFFF);

  vtty_write(0,"Initialize keyboard\n\n",21);
  keyboard_init();

  //shell();
//  puts("Running embedded shell\n");
//  new_task(shell,0x180000,0);

  new_task(time_task,0x140000,0);
  //new_task(time1_task,0x150000,0);
//  new_task(time2_task,0x160000,0);

  while(1);
 }
