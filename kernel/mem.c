#include "paging.h"
#include "mem.h"
#include "syscounters.h"
#include "stdio.h"

extern memory_t memory;

unsigned long get_memory_count() {
    char *pointer = (char*)0x800000,  //8 Megabytes
          old_value;
    while(1) {
        old_value = *pointer;
        *pointer = 0xAA;
        if((*pointer) == (char)0xAA) {
            *pointer = old_value;
            pointer += 0x1000;
        }
        else {
            *pointer = old_value;
            memory.mem_count = (uint32_t)pointer;
            return memory.mem_count;
        }
      }
 }

void kam_init(void) {
    memset((void*)KAM_ADDR, 0, HEEP_START - KAM_ADDR - 0x1000);
}

/*
void *kealloc(unsigned long size)
 {
  if(!size)return (void*)0;
  unsigned long *UPMM=UPMM_ADDR;
  unsigned long pages=(size>>12)+1,mask,founded,addr;
  KAM *kam=(KAM*)KAM_ADDR;

  unsigned long i,j,n=0;
  for(i=0;i!=32768;++i)
   {
    for(j=0,mask=0x1;j!=32;++j,mask=mask<<1)
     {
      if(!(UPMM[i]&mask) && ++founded)
       {
        if(founded==pages)
         {
          addr=0x1000*((i<<5)+j-pages+1);
          for(n=0;n!=pages;++n)assign_page(addr+(n<<12),addr+(n<<12),1,0);
          return addr;
         }
       }
      else founded=0;
     }
   }
  return (void*)0;
 }
*/
void *kealloc(unsigned long size) {
    if(!size) {
        return (void*)0;
    }
    int pages_cnt = size >> 12;          //Pages neaded for allocating
    if(!pages_cnt) {
        pages_cnt = 1;
    }
    int page_part = size % 4096;         //Using of last page

printf("pages_cnt=%i\npage_part=%i\n", pages_cnt, page_part);

    kam_t *kam = (kam_t*)KAM_ADDR;
    int i, j, prev_part;
    unsigned long page;
    for(i = 0; i != 57088; ++i) {                   // Passing KAM array
        prev_part = kam[i].size % 4096;
        if(prev_part || !kam[i].size) {             // If in page present free space
            page = find_free_page_sq(pages_cnt);
            printf("pages_sq=%i\n", page);
            if(page == -1) {
                puts("Not found\n");
                continue;
            }
            kam[i].page = page;
            kam[i].addr = page * 4096 + page_part;
            kam[i].size = size;
            kam[i].pages_used = pages_cnt;
            for(j = 0; j != pages_cnt; ++j) assign_page((void*)((page + j) * 4096), (void*)((page + j) * 4096), 1, 0);
printf("Pages count: %i\nPage: %i\nAddr: %X\nSize: %i\n", pages_cnt, kam[i].page,kam[i].addr, kam[i].size);
            return (void *)kam[i].addr;
        }
    }
}
