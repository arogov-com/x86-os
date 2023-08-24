#include "paging.h"
#include "syscounters.h"

void pde_init(void)
 {
  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;
  unsigned int i;
  for(i=0;i!=1024;++i)
   {
    *(unsigned long*)&pde[i]=0;
    pde[i].address=(PTE_ADDR+i*0x1000)>>12;
   }
  load_pde((void*)PDE_ADDR);
 }

void get_pde_entry(int number,PAGE_DIR *store)
 {
  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;
  *(unsigned long*)store=*(unsigned long*)&pde[number];
 }
void get_pte_entry(int table,int page,PAGE_TABLE *store)
 {
  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;
  PAGE_TABLE *pte=(PAGE_TABLE*)(pde[table].address<<12);
  *(unsigned long*)store=*(unsigned long*)&pte[page];
 }

void load_pde(void *addr)
 {
  asm("movl %0,%%CR3"::"a"(addr));
 }

void paging_enable(int enable)
 {
  asm("movl %CR0,%eax");
  if(enable)asm("orl $0x80000000,%eax");
  else asm("andl $0x7FFFFFFF,%eax");
  asm("movl %eax,%CR0");
 }

int assign_page(void *virtual,void *physical,int writable,int user)
 {
  unsigned long table=(unsigned long)virtual>>22,
                page=((unsigned long)virtual>>12)&0x3FF;

  unsigned long *UPMM=UPMM_ADDR;
  
  unsigned long pn=(table<<10)+page;
  if((UPMM[pn>>5] & (1UL<<(pn%32))))return 0;
  UPMM[pn>>5]=UPMM[pn>>5]|(1UL<<(pn%32));

  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;

  pde[table].present=1;
  pde[table].user=user;
  pde[table].writable=writable;

  PAGE_TABLE *pte=(PAGE_TABLE*)(pde[table].address<<12);

  pte[page].present=1;
  pte[page].user=user;
  pte[page].writable=writable;
  pte[page].address=((unsigned long)physical)>>12;
  ++memory.pages;
  return 1;
 }

void free_page(void *virtual)
 {
  unsigned long table=(unsigned long)virtual>>22,
                page=((unsigned long)virtual>>12)&0x3FF;
  unsigned long *UPMM=UPMM_ADDR;
  
  unsigned long pn=(table<<10)+page;
  UPMM[pn>>5]=UPMM[pn>>5] & (~(1UL<<(pn%32)));

  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;
  PAGE_TABLE *pte=(PAGE_TABLE*)(pde[table].address<<12);
  *(unsigned long*)&pte[page]=0;
  --memory.pages;
 }

void *get_physical_address(void *virtual)
 {
  unsigned long table=(unsigned long)virtual>>22,
                page=((unsigned long)virtual>>12)&0x3FF;
  unsigned long *UPMM=UPMM_ADDR;
  unsigned long pn=(table<<10)+page;
  if(!(UPMM[pn>>5] & (1UL<<(pn%32))))return -1;
  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;
  PAGE_TABLE *pte=(PAGE_TABLE*)(pde[table].address<<12);
  return pte[page].address<<12;
 }

unsigned long get_virtual_addresses(void *physical,unsigned long *virtuals_array)
 {
  PAGE_DIR *pde=(PAGE_DIR*)PDE_ADDR;
  PAGE_TABLE *pte;
  unsigned long i,j,c=0;
  for(i=0;i!=1024;++i)
   {
    if(pde[i].present)
     for(j=0;j!=1024;++j)
      {
       pte=(PAGE_TABLE*)(pde[i].address<<12);
       if(pte[j].present && (pte[j].address<<12)==physical)
        virtuals_array[c++]=(i<<22)|(j<<12);
      }
   }
  return c;
 }

unsigned long get_used_pages(void)
 {
  return memory.pages;
 }

unsigned long find_free_page_sq(unsigned long count)
 {
  if(!count)return (unsigned long)-1;
  unsigned long *UPMM=UPMM_ADDR;
  unsigned long mask,founded=0,addr;
  unsigned long i,j,n=0;
  for(i=0;i!=32768;++i)
   {
    for(j=0,mask=0x1;j!=32;++j,mask=mask<<1)
     {
      if(!(UPMM[i]&mask) && ++founded)
       {
        if(founded==count)return ((i<<5)+j-count+1);
       }
      else founded=0;
     }
   }
  return (unsigned long)-1;
 }
