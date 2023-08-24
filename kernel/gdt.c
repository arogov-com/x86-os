#include "gdt.h"

GDTR gdtr;
char gdt_reg[6];

void lgdt(void *address,unsigned short limit)
 {
  asm("cli");
  GDTR gdtr;
  gdtr.limit=limit;
  gdtr.address=address;
  asm("lgdt 0(,%0,)"::"a"(&gdtr));
  asm("sti");
 }

int set_descriptor(void *gdt_addr,unsigned short num,void *base,unsigned long limit,int present,int DPL,int SB,int granularity,int flags)
 {
  if(num>8192)return 0;
  GDTR *gdtr;
  GDT *gdt;
  if(!gdt_addr)
   {
#ifdef __WIN32__
    asm("sgdt _gdt_reg");
#else
    asm("sgdt gdt_reg");
#endif
    gdtr=(GDTR*)gdt_reg;
    gdt=gdtr->address;
   }
  else gdt=(GDT*)gdt_addr;

  gdt[num].low_limit=(unsigned short)limit;
  gdt[num].low_base=(unsigned long)base;
  gdt[num].accessed=0;
  gdt[num].flags=flags;
  gdt[num].DPL=DPL;
  gdt[num].present=present;
  gdt[num].high_limit=(limit>>16);
  gdt[num].avl=0;
  gdt[num].reserved=0;
  gdt[num].SB=SB;
  gdt[num].granularity=granularity;
  gdt[num].high_base=(unsigned char)(((unsigned long)base)>>24);
  return 1;
 }

int get_gdt(int num,GDT *store)
 {
  GDTR *gdtr;
#ifdef __WIN32__
  asm("sgdt _gdt_reg");
#else
  asm("sgdt gdt_reg");
#endif
  gdtr=(GDTR*)gdt_reg;
  if(num*sizeof(GDT) > gdtr->limit+1)return 0;
  GDT *gdt=gdtr->address;
  memcpy(store,&gdt[num],sizeof(GDT));
  return 1;
 }
