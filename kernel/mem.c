#include <stddef.h>
#include "paging.h"
#include "mem.h"
#include "syscounters.h"
#include "stdio.h"

extern memory_t memory;
kam_t *kam;
int kam_size;

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

void kam_init(void *kam_address, unsigned long size) {
    kam = (kam_t *)kam_address;
    kam_size = size;
    memset(kam_address, 0, size);
}

void *kealloc(unsigned long size) {
    if(!size) {
        return NULL;
    }

    int pages_cnt = size >> 12;  // Number of pages needed for allocation
    if(size & 0xfff) {
        ++pages_cnt;
    }

    int i, j;
    unsigned long page;
    for(i = 0; i != KAM_SIZE; ++i) {  // Passing KAM array
        if(!kam[i].addr) {
            page = find_free_page_sq(pages_cnt);
            if(page == -1) {
                return NULL;
            }
            kam[i].page = page;
            kam[i].addr = (void *)(page << 12);
            kam[i].size = size;
            kam[i].pages_used = pages_cnt;
            kam[i].used = (size >> 5) + (size & 0x1f ? 1 : 0);
            for(j = 0; j != pages_cnt; ++j) {
                assign_page((void*)((page + j) << 12), (void*)((page + j) << 12), 1, 0);
            }
            memory.mem_used += size;
            return kam[i].addr;
        }
        else if(kam[i].addr && (int)size <= 4096 - (kam[i].used << 5)) {
            void *paddr = kam[i].addr + (kam[i].used << 5);
            for(j = 0; j != KAM_SIZE; ++j) {
                if(!kam[j].addr) {
                    kam[i].used += (size >> 5) + (size & 0x1f ? 1 : 0);
                    kam[j].page = kam[i].page;
                    kam[j].addr = paddr;
                    kam[j].size = size;
                    kam[j].pages_used = 1;
                    kam[j].used = 0xff;
                    memory.mem_used += size;
                    return paddr;
                }
            }
        }
    }
    return NULL;
}

void kefree(void *ptr) {
    if(!ptr) {
        return;
    }
    kam_t *kam = (kam_t*)KAM_ADDR;
    int i, j;
    for(i = 0; i != KAM_SIZE; ++i) {
        if(kam[i].addr == ptr) {
            for(j = 0; j != kam[i].pages_used; ++j) {
                free_page(kam[i].addr + j * 4096);
            }
            memset(&kam[i], 0, sizeof(kam_t));
            memory.mem_used -= kam[i].size;
            return;
        }
        else if(ptr > kam[i].addr && ptr < kam[i].addr + 4096) {
            for(j = 0; j != KAM_SIZE; ++j) {
                if(kam[j].addr == ptr) {
                    memory.mem_used -= kam[j].size;
                    kam[i].used -= (kam[j].size >> 5);
                    memset(&kam[j], 0, sizeof(kam_t));
                    if(!kam[i].used) {
                        free_page((void *)kam[i].page);
                        memset(&kam[i], 0, sizeof(kam_t));
                        return;
                    }
                    return;
                }
            }
        }
    }
}
