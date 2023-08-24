#include <stddef.h>
#include "paging.h"
#include "syscounters.h"

unsigned int invlpg_addr;

extern memory_t memory;

void pde_init(void) {
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    unsigned int i;
    memset(pde, 0, 1024 * sizeof(page_dir_t));
    for(i = 0; i != 1024; ++i) pde[i].address = (PTE_ADDR + i * 0x1000) >> 12;
    load_pde((void*)PDE_ADDR);
}

void get_pde_entry(int number, page_dir_t *store) {
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    *(unsigned long*)store = *(unsigned long*)&pde[number];
}

void get_pte_entry(int table, int page, PAGE_TABLE *store) {
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    PAGE_TABLE *pte = (PAGE_TABLE*)(pde[table].address << 12);
    *(unsigned long*)store = *(unsigned long*)&pte[page];
}

void load_pde(void *addr) {
    // asm("cli");
    asm volatile("mov %0,%%CR3"::"a"(addr));
    // asm("sti");
}

void paging_enable(int enable) {
    // asm("cli");
    asm volatile("mov %CR0, %eax");
    if(enable) asm volatile("or $0x80000000, %eax");
    else asm volatile("and $0x7FFFFFFF, %eax");
    asm volatile("mov %eax, %CR0");
    // asm("sti");
}

int create_pde(unsigned int number, int mod_if_exists, int options, int cache, void *page_table) {
    if(number > 1023) return BOUND_ERROR;
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    if(pde[number].present && !mod_if_exists) return PDE_ALREADY_EXISTS;
    pde[number].present = 1;

    pde[number].writable = (options >> 2) & 1;
    pde[number].user = (options >> 1) & 1;
    pde[number].size = options & 1;

    pde[number].pcd = cache & 1;
    pde[number].pwt = (cache >> 1) & 1;
    pde[number].global = (cache >> 2) & 1;

    pde[number].accessed = 0;
    pde[number].reserved = 0;
    pde[number].available = 0;

    pde[number].address = ((unsigned int)page_table) >> 12;

    if(mod_if_exists) {
        void *addr = (void*)(number << 22);
        asm volatile("invlpg %0"::"m"(*(char*)addr));
    }

    return 1;
}

int create_pte(unsigned int table, unsigned int page, int mod_if_exists, int options, int cache, void *virtual) {
    if(table > 1023 || page > 1023) return BOUND_ERROR;
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    if(!pde[table].present) return TABLE_NOT_PRESENT;

    PAGE_TABLE *pte = (PAGE_TABLE*)(pde[table].address << 12);

    if(pte[page].present && !mod_if_exists) return PTE_ALREADY_EXISTS;

    pte[page].writable = (options >> 2) & 1;
    pte[page].user = (options >> 1) & 1;
    pte[page].pat = options & 1;

    pte[page].pcd = cache & 1;
    pte[page].pwt = (cache >> 1) & 1;
    pte[page].global = (cache >> 2) & 1;

    pte[page].accessed = 0;
    pte[page].dirty = 0;
    pte[page].available = 0;

    pte[page].address = ((unsigned int)virtual) >> 12;

      if(mod_if_exists) {
          void *addr = (void*)( (table << 22) | (page << 12) );
          asm volatile("invlpg %0"::"m"(*(char*)addr));
    }

    return 1;
 }

int clear_pde(unsigned int number) {
    if(number > 1023) return BOUND_ERROR;
    page_dir_t *pde = (page_dir_t*) PDE_ADDR;
    pde[number].present = 0;
    void *addr = (void*)(number << 22);
    asm volatile("invlpg %0"::"m"(*(char*)addr));
    return 1;
}

int clear_pte(unsigned int table, unsigned int page) {
    if(table > 1023) return BOUND_ERROR;
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    if(!pde[table].present) return TABLE_NOT_PRESENT;
    PAGE_TABLE *pte = (PAGE_TABLE*)(pde[table].address << 12);
    if(!pte[page].present) return 1;
    pte[page].present = 0;
    void *addr = (void*)( (table << 22) | (page << 12) );
    asm volatile("invlpg %0"::"m"(*(char*)addr));
    return 1;
 }

int assign_page(void *virtual, void *physical, int writable, int user) {
    unsigned long table = (unsigned long)virtual >> 22,
                  page = ((unsigned long)virtual >> 12) & 0x3FF;

    unsigned long *UPMM = (void *)UPMM_ADDR;
    unsigned long pn = (table << 10) + page;

    if((UPMM[pn >> 5] & (1UL << (pn % 32)))) return 0;
    UPMM[pn >> 5] = UPMM[pn >> 5] | (1UL << (pn % 32));

    page_dir_t *pde = (page_dir_t*)PDE_ADDR;

    pde[table].present = 1;
    pde[table].user = user;
    pde[table].writable = writable;

    PAGE_TABLE *pte = (PAGE_TABLE*)(pde[table].address << 12);

    pte[page].present = 1;
    pte[page].user = user;
    pte[page].writable = writable;
    pte[page].address = ((unsigned long)physical) >> 12;
    ++memory.pages_used;
    return 1;
 }

void free_page(void *virtual) {
    unsigned long table = (unsigned long)virtual >> 22,
                  page = ((unsigned long)virtual >> 12) & 0x3FF;
    unsigned long *UPMM = (void *)UPMM_ADDR;

    unsigned long pn = (table << 10) + page;
    UPMM[pn >> 5] = UPMM[pn >> 5] & (~(1UL << (pn % 32)));

    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    PAGE_TABLE *pte = (PAGE_TABLE*)(pde[table].address << 12);
    *(unsigned long*)&pte[page] = 0;
    --memory.pages_used;
}

void *get_physical_address(void *virtual) {
  unsigned long table = (unsigned long)virtual >> 22,
                page = ((unsigned long)virtual >> 12) & 0x3FF;
  unsigned long *UPMM = (void *)UPMM_ADDR;
  unsigned long pn = (table << 10) + page;
  if(!(UPMM[pn >> 5] & (1UL << (pn % 32)))) return NULL;
  page_dir_t *pde = (page_dir_t*)PDE_ADDR;
  PAGE_TABLE *pte = (PAGE_TABLE*)(pde[table].address << 12);
  return (void *)(pte[page].address << 12);
 }

unsigned long get_virtual_addresses(void *physical, unsigned long *virtuals_array) {
    page_dir_t *pde = (page_dir_t*)PDE_ADDR;
    PAGE_TABLE *pte;
    unsigned long i, j, c = 0;
    for(i = 0; i != 1024; ++i) {
        if(pde[i].present)
        for(j = 0; j != 1024; ++j) {
            pte = (PAGE_TABLE*)(pde[i].address << 12);
            if(pte[j].present && (void *)(pte[j].address << 12) == physical)
                virtuals_array[c++]=(i<<22)|(j<<12);
        }
    }
    return c;
 }

unsigned long get_used_pages(void) {
    return memory.pages_used;
}

unsigned long find_free_page_sq(unsigned long count) {
    if(!count) return (unsigned long)-1;
    unsigned long *UPMM = (void *)UPMM_ADDR;
    unsigned long mask, founded = 0, addr;
    unsigned long i, j, n = 0;
    for(i = 0; i != 32768; ++i) {
        for(j = 0, mask = 0x1; j != 32; ++j, mask = mask << 1) {
            if(!(UPMM[i] & mask) && ++founded) {
                if(founded == count) return ((i << 5) + j - count + 1);
            }
            else founded=0;
        }
    }

    return (unsigned long)-1;
}
